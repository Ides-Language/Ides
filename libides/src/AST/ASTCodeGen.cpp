#include <ides/AST/AST.h>
#include <ides/AST/ASTExpression.h>
#include <ides/AST/ASTConstantExpression.h>

#include <boost/uuid/uuid_generators.hpp> // generators
#include <boost/uuid/uuid_io.hpp>         // streaming operators etc.

#include <llvm/BasicBlock.h>
#include <llvm/Function.h>
#include "llvm/Analysis/Verifier.h"

#include <sstream>
#include <queue>

namespace Ides {
namespace AST {
        
    void ASTCompilationUnit::Compile(ParseContext& ctx) {
        for (auto i = this->begin(); i != this->end(); ++i) {
            if (ASTFunction* f = dynamic_cast<ASTFunction*>(*i)) {
                ctx.GetPublicSymbols()->insert(std::make_pair(f->GetMangledName(), f));
            }
        }
        
        std::queue<ASTFunction*> functions;
        for (auto i = this->begin(); i != this->end(); ++i) {
            (*i)->GetValue(ctx);
            if (ASTFunction* func = dynamic_cast<ASTFunction*>(*i)) {
                ctx.GetModule()->getOrInsertFunction(func->name->name, (llvm::FunctionType*)func->GetType(ctx)->GetLLVMType(ctx));
                
                if (func->body || func->val)
                    functions.push(func);
            }
        }
        
        while (!functions.empty()) {
            functions.front()->GenBody(ctx);
            functions.pop();
        }
    }
    
    const Ides::Types::Type* ASTFunction::GetType(ParseContext& ctx)
    {
        if (evaluatingtype) {
            throw Ides::Diagnostics::CompileError("recursive functions cannot infer types", this->val->exprloc);
        }
        evaluatingtype = true;
        
        ctx.PushLocalScope();
        
        std::vector<const Ides::Types::Type*> argTypes;
        if (this->args) {
            for (auto i = this->args->begin(); i != this->args->end(); ++i) {
                ASTDeclaration* decl = dynamic_cast<ASTDeclaration*>(*i);
                ctx.GetLocalSymbols()->insert(std::make_pair(decl->name->name, decl));
                argTypes.push_back((*i)->GetType(ctx));
            }
        }
        
        const Ides::Types::Type* ret = NULL;
        
        if (this->rettype == NULL) {
            if (val == NULL) return Ides::Types::VoidType::GetSingletonPtr();
            else {
                try {
                    ret = val->GetType(ctx);
                } catch (const Ides::Diagnostics::CompileError& ex) {
                    this->evaluatingtype = false;
                    throw Ides::Diagnostics::CompileError(ex.message(), this->val->exprloc, ex);
                }
            }
        } else {
            ret = this->rettype->GetType(ctx);
        }
        evaluatingtype = false;
        assert(ret != NULL);
        return Ides::Types::FunctionType::Get(ret, argTypes);
    }
    
    llvm::Value* ASTFunction::GetValue(ParseContext& ctx)
    {
        if (func == NULL) {
            llvm::FunctionType *FT = static_cast<llvm::FunctionType*>(this->GetType(ctx)->GetLLVMType(ctx));
            func = llvm::Function::Create(FT, llvm::GlobalValue::ExternalLinkage, this->name->name);
        }
        return func;
    }
    
    void ASTFunction::GenBody(ParseContext& ctx)
    {
        assert(func != NULL && (this->val != NULL || this->body != NULL));
        
        ctx.PushLocalScope();
        
        auto i = args->begin();
        llvm::Function::arg_iterator ai = func->arg_begin();
        for (; i != args->end() && ai != func->arg_end(); ++ai, ++i) {
            ASTDeclaration* decl = dynamic_cast<ASTDeclaration*>(*i);
            decl->val = ai;
            ai->setName(decl->name->name);
            ctx.GetLocalSymbols()->insert(std::make_pair(decl->name->name, decl));
        }
        assert (i == args->end() && ai == func->arg_end());
        
        llvm::BasicBlock* entryblock = llvm::BasicBlock::Create(ctx.GetIRBuilder()->getContext(), "entry", func);
        ctx.GetIRBuilder()->SetInsertPoint(entryblock);
        if (this->val) {
            ctx.GetIRBuilder()->CreateRet(this->val->GetValue(ctx));
        }
        else if (this->body) {
            llvm::BasicBlock* exitblock = llvm::BasicBlock::Create(ctx.GetIRBuilder()->getContext());
            
            for (auto i = this->body->begin(); i != this->body->end(); ++i) {
                (*i)->GetValue(ctx);
            }
        }
        llvm::verifyFunction(*func);
        func->dump();
        
        ctx.PopLocalScope();
    }
    
    const Ides::Types::Type* ASTFunctionCall::GetType(ParseContext& ctx) {
        const Ides::Types::FunctionType* fntype = dynamic_cast<const Ides::Types::FunctionType*>(fn->GetType(ctx));
        if (fn == NULL) throw Ides::Diagnostics::CompileError("cannot call a non-function expression", this->exprloc);
        return fntype->retType;
    }
    
    llvm::Value* ASTFunctionCall::GetValue(ParseContext& ctx) {
        const Ides::Types::FunctionType* function = static_cast<const Ides::Types::FunctionType*>(fn->GetType(ctx));
        llvm::Function* func = static_cast<llvm::Function*>(fn->GetValue(ctx));
        std::vector<llvm::Value*> fnargs;
        if (args) {
            auto i = args->begin();
            auto defi = function->argTypes.begin();
            for (; i != args->end() && defi != function->argTypes.end(); ++i, ++defi) {
                if (!(*i)->GetType(ctx)->IsEquivalentType(*defi)) {
                    std::stringstream err;
                    err << "expecting argument of type '" << (*defi)->type_name << "' at: " << this->exprloc.GetText();
                    throw Ides::Diagnostics::CompileError(err.str(), (*i)->exprloc);
                }
                fnargs.push_back((*i)->GetValue(ctx));
            }
        }
        return ctx.GetIRBuilder()->CreateCall(func, fnargs);
    }
    
    llvm::Value* ASTIdentifier::GetValue(ParseContext& ctx) {
        AST* ret = ctx.GetLocalSymbols()->LookupRecursive(this->name);
        if (ret == NULL) throw Ides::Diagnostics::CompileError("no such identifier " + this->name, this->exprloc);
        return ret->GetValue(ctx);
    }
    
    const Ides::Types::Type* ASTIdentifier::GetType(ParseContext& ctx) {
        AST* ret = ctx.GetLocalSymbols()->LookupRecursive(this->name);
        if (ret == NULL) throw Ides::Diagnostics::CompileError("no such identifier " + this->name, this->exprloc);
        return ret->GetType(ctx);
    }
    
    const Ides::Types::Type* ASTTypeName::GetType(ParseContext& ctx) {
        throw Ides::Diagnostics::CompileError("no such type " + this->name->name, this->exprloc);
    }
    
}
}