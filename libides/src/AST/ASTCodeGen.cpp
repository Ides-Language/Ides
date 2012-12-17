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
        
    void ASTCompilationUnit::Compile(llvm::Module &mod) {
        llvm::IRBuilder<> builder(mod.getContext());
        
        SymbolTable* globals = new SymbolTable();
        
        for (auto i = this->begin(); i != this->end(); ++i) {
            if (ASTFunction* f = dynamic_cast<ASTFunction*>(*i)) {
                globals->insert(std::make_pair(f->GetMangledName(), f));
            }
        }
        
        std::queue<ASTFunction*> functions;
        for (auto i = this->begin(); i != this->end(); ++i) {
            (*i)->GetValue(&builder, *globals);
            if (ASTFunction* func = dynamic_cast<ASTFunction*>(*i)) {
                mod.getOrInsertFunction(func->name->name, (llvm::FunctionType*)func->GetType(&builder, *globals)->GetLLVMType(&builder));
                
                if (func->body || func->val)
                    functions.push(func);
            }
        }
        
        mod.dump();
        while (!functions.empty()) {
            functions.front()->GenBody(&builder, *globals);
            functions.pop();
        }
        
        delete globals;
    }
    
    llvm::Value* ASTFunction::GetValue(llvm::IRBuilder<>* builder, SymbolTable& scope)
    {
        if (func == NULL) {
            llvm::FunctionType *FT = static_cast<llvm::FunctionType*>(this->GetType(builder, scope)->GetLLVMType(builder));
            func = llvm::Function::Create(FT, llvm::GlobalValue::ExternalLinkage, this->name->name);
        }
        return func;
    }
    
    const Ides::Types::Type* ASTFunction::GetType(llvm::IRBuilder<>* builder, SymbolTable& scope)
    {
        if (evaluatingtype) {
            throw Ides::Diagnostics::CompileError("recursive functions cannot infer types", this->val->exprloc);
        }
        evaluatingtype = true;
        
        SymbolTable argsymbols;
        argsymbols.SetParentScope(&scope);
        
        std::vector<const Ides::Types::Type*> argTypes;
        if (this->args) {
            for (auto i = this->args->begin(); i != this->args->end(); ++i) {
                ASTDeclaration* decl = dynamic_cast<ASTDeclaration*>(*i);
                argsymbols.insert(std::make_pair(decl->name->name, decl));
                argTypes.push_back((*i)->GetType(builder, scope));
            }
        }
        
        const Ides::Types::Type* ret = NULL;
        
        if (this->rettype == NULL) {
            if (val == NULL) return Ides::Types::VoidType::GetSingletonPtr();
            else {
                try {
                    ret = val->GetType(builder, argsymbols);
                } catch (const Ides::Diagnostics::CompileError& ex) {
                    this->evaluatingtype = false;
                    throw Ides::Diagnostics::CompileError(ex.message(), this->val->exprloc, ex);
                }
            }
        } else {
            ret = this->rettype->GetType(builder, argsymbols);
        }
        evaluatingtype = false;
        assert(ret != NULL);
        return Ides::Types::FunctionType::Get(ret, argTypes);
    }
    
    void ASTFunction::GenBody(llvm::IRBuilder<>* builder, SymbolTable& scope)
    {
        assert(func != NULL && (this->val != NULL || this->body != NULL));
        
        SymbolTable locals;
        locals.SetParentScope(&scope);
        
        auto i = args->begin();
        llvm::Function::arg_iterator ai = func->arg_begin();
        for (; i != args->end() && ai != func->arg_end(); ++ai, ++i) {
            ASTDeclaration* decl = dynamic_cast<ASTDeclaration*>(*i);
            decl->val = ai;
            ai->setName(decl->name->name);
            locals.insert(std::make_pair(decl->name->name, decl));
        }
        assert (i == args->end() && ai == func->arg_end());
        
        llvm::BasicBlock* entryblock = llvm::BasicBlock::Create(builder->getContext(), "entry", func);
        builder->SetInsertPoint(entryblock);
        if (this->val) {
            builder->CreateRet(this->val->GetValue(builder, locals));
        }
        else if (this->body) {
            llvm::BasicBlock* exitblock = llvm::BasicBlock::Create(builder->getContext());
            
            for (auto i = this->body->begin(); i != this->body->end(); ++i) {
                (*i)->GetValue(builder, locals);
            }
        }
        llvm::verifyFunction(*func);
        func->dump();
    }
    
    const Ides::Types::Type* ASTFunctionCall::GetType(llvm::IRBuilder<>* builder, SymbolTable& scope) {
        const Ides::Types::FunctionType* fntype = dynamic_cast<const Ides::Types::FunctionType*>(fn->GetType(builder, scope));
        if (fn == NULL) throw Ides::Diagnostics::CompileError("cannot call a non-function expression", this->exprloc);
        return fntype->retType;
    }
    
    llvm::Value* ASTFunctionCall::GetValue(llvm::IRBuilder<> *builder, Ides::AST::SymbolTable &scope) {
        llvm::Function* func = static_cast<llvm::Function*>(fn->GetValue(builder, scope));
        std::vector<llvm::Value*> fnargs;
        if (args) {
            for (auto i = args->begin(); i != args->end(); ++i) {
                fnargs.push_back((*i)->GetValue(builder, scope));
            }
        }
        return builder->CreateCall(func, fnargs);
    }
    
    llvm::Value* ASTIdentifier::GetValue(llvm::IRBuilder<>* builder, SymbolTable& scope) {
        AST* ret = scope.LookupRecursive(this->name);
        if (ret == NULL) throw Ides::Diagnostics::CompileError("no such identifier " + this->name, this->exprloc);
        return ret->GetValue(builder, scope);
    }
    
    const Ides::Types::Type* ASTIdentifier::GetType(llvm::IRBuilder<>* builder, SymbolTable& scope) {
        AST* ret = scope.LookupRecursive(this->name);
        if (ret == NULL) throw Ides::Diagnostics::CompileError("no such identifier " + this->name, this->exprloc);
        return ret->GetType(builder, scope);
    }
    
    const Ides::Types::Type* ASTTypeName::GetType(llvm::IRBuilder<>* builder, SymbolTable& scope) {
        throw Ides::Diagnostics::CompileError("no such type " + this->name->name, this->exprloc);
    }
    
}
}