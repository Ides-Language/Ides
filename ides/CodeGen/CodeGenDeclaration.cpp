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
            last = vi->second;
            return;
        }
        
        llvm::Value* var = builder->CreateAlloca(this->GetLLVMType(ast->GetType(actx)), 0, ast->GetName());
        variables.insert(std::make_pair(ast, var));
        if (ast->initval != NULL) {
            builder->CreateStore(GetValue(ast->initval, ast->GetType(actx)), var);
        }
        
        actx.GetCurrentScope()->AddMember(ast->GetName(), ast);
        
        last = var;
    }
    
    void CodeGen::Visit(Ides::AST::StructDeclaration* ast) { SETTRACE("CodeGen::Visit(StructDeclaration)")
        Ides::Types::StructType* st = Ides::Types::StructType::GetOrCreate(actx, ast->GetName());
        
        std::vector<llvm::Type*> memberLLVMtypes;
        std::vector<std::pair<Ides::String, const Ides::Types::Type*> > membertypes;
        for (auto i = ast->members.begin(); i != ast->members.end(); ++i) {
            Ides::AST::NamedDeclaration* decl = (Ides::AST::NamedDeclaration*)*i;
            const Ides::Types::Type* memberType = decl->GetType(actx);
            membertypes.push_back(std::make_pair(decl->GetName(), memberType));
            st->AddMember(decl->GetName(), decl);
            memberLLVMtypes.push_back(this->GetLLVMType(memberType));
        }
        st->SetMembers(actx, membertypes);
        static_cast<llvm::StructType*>(this->GetLLVMType(st))->setBody(memberLLVMtypes, false);
    }
    
    void CodeGen::Visit(Ides::AST::FunctionDeclaration* ast) { SETTRACE("CodeGen::Visit(FunctionDeclaration)")
        
        auto fi = functions.find(ast);
        if (fi != functions.end()) {
            last = fi->second;
            return;
        }
        Ides::AST::ASTContext::DeclScope typescope(actx, ast);
        
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
        
        llvm::FunctionType *FT = static_cast<llvm::FunctionType*>(this->GetLLVMType(ast->GetType(actx)));
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
            ai->setName(decl->GetName());
            decl->Accept(this);
            builder->CreateStore(ai, last);
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
                    builder->CreateRet(GetValue(ast->val));
                } else if (valtype->HasImplicitConversionTo(rettype)) {
                    builder->CreateRet(GetValue(ast->val));
                } else {
                    func->removeFromParent();
                    throw detail::CodeGenError(*diag, NO_IMPLICIT_CONVERSION, ast->val->exprloc) << rettype->ToString() << valtype->ToString();
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
                    func->removeFromParent();
                    throw detail::CodeGenError(*diag, FUNCTION_NO_RETURN, ast->exprloc);
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
