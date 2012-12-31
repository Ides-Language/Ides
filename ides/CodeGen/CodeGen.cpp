//
//  CodeGen.cpp
//  ides
//
//  Created by Sean Edwards on 12/30/12.
//
//

#include "CodeGen.h"
#include "llvm/Analysis/Verifier.h"
#include <ides/Diagnostics/Diagnostics.h>

#include <ides/AST/ConstantExpression.h>


namespace Ides {
namespace CodeGen {
    
    using namespace Ides::Diagnostics;
    
    CodeGen::CodeGen(clang::IntrusiveRefCntPtr<clang::DiagnosticsEngine> diags, llvm::LLVMContext& lctx, Ides::AST::ASTContext& actx)
        : lctx(lctx), actx(actx), diag(diags)
    {
        this->module = new llvm::Module("Ides Module", lctx);
        this->builder = new llvm::IRBuilder<>(lctx);
    }
    
    CodeGen::~CodeGen() {
        delete builder;
        delete module;
    }
    
    void CodeGen::Compile(Ides::AST::CompilationUnit* ast) { SETTRACE("CodeGen::Compile")
        try {
            ast->Accept(this);
            module->dump();
        } catch (const detail::CodeGenError&) {
            std::cerr << "Build completed with errors." << std::endl;
        }
        functions.clear();
    }
    
    void CodeGen::Visit(Ides::AST::VariableDeclaration* ast) { SETTRACE("CodeGen::Visit(VariableDeclaration)")
        auto vi = variables.find(ast);
        if (vi != variables.end()) {
            last = vi->second;
            return;
        }
        
        llvm::Value* var = builder->CreateAlloca(ast->GetType(actx)->GetLLVMType(actx), 0, ast->GetName());
        if (ast->initval != NULL) {
            ast->initval->Accept(this);
            builder->CreateStore(last, var);
        }
        last = var;
    }
    
    void CodeGen::Visit(Ides::AST::StructDeclaration* ast) { SETTRACE("CodeGen::Visit(StructDeclaration)")
        ast->GenType(actx);
    }
    
    void CodeGen::Visit(Ides::AST::FunctionDeclaration* ast) { SETTRACE("CodeGen::Visit(FunctionDeclaration)")
        auto fi = functions.find(ast);
        if (fi != functions.end()) {
            last = fi->second;
            return;
        }
        
        struct FSM {
            FSM(CodeGen* cg, Ides::AST::FunctionDeclaration* ast) : cg(cg) {
                cg->currentFunctions.push(ast);
            }
            
            ~FSM() throw() {
                cg->currentFunctions.pop();
            }
            CodeGen* cg;
        };
        FSM functionStackManager(this, ast);
        
        llvm::FunctionType *FT = static_cast<llvm::FunctionType*>(ast->GetType(actx)->GetLLVMType(actx));
        //func = (llvm::Function*)ctx.GetModule()->getOrInsertFunction(this->GetMangledName(), FT);
        llvm::Function* func = llvm::Function::Create(FT, llvm::GlobalValue::ExternalLinkage, ast->GetName(), module);
        func->setGC("shadow-stack");
        
        if (ast->GetReturnType(actx)->IsEquivalentType(Ides::Types::UnitType::GetSingletonPtr())) {
            func->addFnAttr(llvm::Attributes::NoReturn);
        }
        
        functions.insert(std::make_pair(ast, func));
        last = func;
        
        if (ast->val == NULL && ast->body == NULL) return;
        
        llvm::BasicBlock* entryblock = llvm::BasicBlock::Create(lctx, "entry", func);
        builder->SetInsertPoint(entryblock);
        
        auto i = ast->GetArgs().begin();
        llvm::Function::arg_iterator ai = func->arg_begin();
        for (; i != ast->GetArgs().end() && ai != func->arg_end(); ++ai, ++i) {
            Ides::AST::VariableDeclaration* decl = i->get();
            decl->Accept(this);
            
            ai->setName(decl->GetName());
        }
        assert (i == ast->GetArgs().end() && ai == func->arg_end());
        
        
        if (ast->val) {
            if (ast->GetReturnType(actx)->IsEquivalentType(Ides::Types::VoidType::GetSingletonPtr())
                || ast->GetReturnType(actx)->IsEquivalentType(Ides::Types::UnitType::GetSingletonPtr())) {
                ast->val->Accept(this);
                builder->CreateRetVoid();
            } else {
                const Ides::Types::Type* valtype = ast->val->GetType(actx);
                const Ides::Types::Type* rettype = ast->GetReturnType(actx);
                if (valtype->IsEquivalentType(rettype)) {
                    ast->val->Accept(this);
                    builder->CreateRet(this->last);
                } else if (valtype->HasImplicitConversionTo(rettype)) {
                    try {
                        ast->val->Accept(this);
                        builder->CreateRet(this->last);
                    } catch (const std::runtime_error& ex) {
                        // TODO: Diagnostics!
                    }
                } else {
                    Diag(NO_IMPLICIT_CONVERSION, ast->val->exprloc.getBegin()) << rettype->ToString() << valtype->ToString();
                    func->removeFromParent();
                    throw detail::CodeGenError();
                }
            }
        }
        else if (ast->body) {
            try {
                ast->body->Accept(this);
                if (ast->GetReturnType(actx)->IsEquivalentType(Ides::Types::VoidType::GetSingletonPtr())) {
                    // Function didn't return, but it's void, so NBD.
                    builder->CreateRetVoid();
                } else {
                    Diag(FUNCTION_NO_RETURN, ast->exprloc.getBegin());
                    func->removeFromParent();
                    throw detail::CodeGenError();
                }
            } catch (const detail::UnitValueException&) {
                // Function returned.
            }
        }
        
        llvm::verifyFunction(*func);
        
        last = func;
    }
    
    void CodeGen::Visit(Ides::AST::ReturnExpression* ast) { SETTRACE("CodeGen::Visit(ReturnExpression)")
        const Ides::Types::Type* exprtype = ast->GetRetType(actx);
        const Ides::Types::Type* funcrettype = this->currentFunctions.top()->GetReturnType(actx);
        if (funcrettype == Ides::Types::VoidType::GetSingletonPtr()) {
            if (ast->GetRetVal() != NULL) {
                Diag(RETURN_FROM_VOID, ast->exprloc.getBegin());
                throw detail::CodeGenError();
            }
            last = builder->CreateRetVoid();
        }
        else if (funcrettype == Ides::Types::UnitType::GetSingletonPtr()) {
            Diag(RETURN_FROM_UNIT, ast->exprloc.getBegin());
            throw detail::CodeGenError();
        }
        else if (ast->GetRetVal() == NULL && funcrettype != Ides::Types::VoidType::GetSingletonPtr()) {
            Diag(RETURN_NO_EXPRESSION, ast->exprloc.getBegin());
            throw detail::CodeGenError();
        }
        else if (exprtype->IsEquivalentType(funcrettype)) {
            ast->GetRetVal()->Accept(this);
            builder->CreateRet(last);
        }
        else {
            try {
                //ctx.GetIRBuilder()->CreateRet(this->retval->GetValue(ctx, funcrettype));
            } catch (const std::exception& ex) {
                //throw Ides::Diagnostics::CompileError(ex.what(), this->exprloc);
            }
        }
        throw detail::UnitValueException();
    }
    
    
    
    void CodeGen::Visit(Ides::AST::ConstantStringExpression* ast) {
        
    }
    
    void CodeGen::Visit(Ides::AST::ConstantCStringExpression* ast) {
        last = builder->CreateGlobalStringPtr(ast->GetBuffer().str());
    }
    
    void CodeGen::Visit(Ides::AST::ConstantWCStringExpression* ast) {
        
    }
    
    void CodeGen::Visit(Ides::AST::ConstantLCStringExpression* ast) {
        
    }
    
    void CodeGen::Visit(Ides::AST::ConstantBoolExpression* ast) {
        last = ast->GetValue() ? llvm::ConstantInt::getTrue(lctx) : llvm::ConstantInt::getFalse(lctx);
    }
    
    void CodeGen::Visit(Ides::AST::ConstantCharExpression* ast) {
        last = llvm::ConstantInt::get(ast->GetType(actx)->GetLLVMType(actx), ast->GetValue());
    }
    
    void CodeGen::Visit(Ides::AST::ConstantIntExpression* ast) {
        last = llvm::ConstantInt::get(ast->GetType(actx)->GetLLVMType(actx), ast->GetValue());
    }
    
    void CodeGen::Visit(Ides::AST::ConstantFloatExpression* ast) {
        last = llvm::ConstantFP::get(ast->GetType(actx)->GetLLVMType(actx), ast->GetValue());
    }
}
}