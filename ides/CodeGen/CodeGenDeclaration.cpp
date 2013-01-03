//
//  CodeGenDeclaration.cpp
//  ides
//
//  Created by Sean Edwards on 1/2/13.
//
//

#include "CodeGen.h"
#include "llvm/Analysis/Verifier.h"
#include <ides/Diagnostics/Diagnostics.h>

#include <ides/AST/Statement.h>
#include <llvm/BasicBlock.h>


namespace Ides {
namespace CodeGen {
    
    
    using namespace Ides::Diagnostics;
    
    void CodeGen::Visit(Ides::AST::VariableDeclaration* ast) { SETTRACE("CodeGen::Visit(VariableDeclaration)")
        auto vi = variables.find(ast);
        if (vi != variables.end()) {
            last = builder->CreateLoad(vi->second);
            return;
        }
        
        llvm::Value* var = builder->CreateAlloca(ast->GetType(actx)->GetLLVMType(actx), 0, ast->GetName());
        variables.insert(std::make_pair(ast, var));
        if (ast->initval != NULL) {
            ast->initval->Accept(this);
            builder->CreateStore(last, var);
        }
        
        actx.GetCurrentScope()->AddMember(ast->GetName(), ast);
        
        last = var;
    }
    
    void CodeGen::Visit(Ides::AST::StructDeclaration* ast) { SETTRACE("CodeGen::Visit(StructDeclaration)")
        ast->GenType(actx);
    }
    
    void CodeGen::Visit(Ides::AST::FunctionDeclaration* ast) { SETTRACE("CodeGen::Visit(FunctionDeclaration)")
        Ides::AST::ASTContext::DeclScope typescope(actx, ast);
        
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
            Ides::AST::VariableDeclaration* decl = *i;
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

    
}
}
