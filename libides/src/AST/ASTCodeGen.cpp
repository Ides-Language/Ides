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
        class RecursiveTypeEvalException : public Ides::Diagnostics::CompileError {
        public:
            RecursiveTypeEvalException(const Ides::String& msg, const Ides::Diagnostics::SourceLocation& loc) :
            Ides::Diagnostics::CompileError(msg, loc) { }
            RecursiveTypeEvalException(const Ides::String& msg, const Ides::Diagnostics::SourceLocation& loc, const Ides::Diagnostics::CompileError& from) :
            Ides::Diagnostics::CompileError(msg, loc, from) { }
            virtual ~RecursiveTypeEvalException() throw() {}
        };
        
        // No need to re-calculate the type.
        if (functype != NULL) return functype;
        
        if (evaluatingtype) {
            throw RecursiveTypeEvalException("recursive functions cannot infer types", this->val->exprloc);
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
            if (val == NULL) ret = Ides::Types::VoidType::GetSingletonPtr();
            else {
                try {
                    ret = val->GetType(ctx);
                } catch (const RecursiveTypeEvalException& ex) {
                    this->evaluatingtype = false;
                    throw RecursiveTypeEvalException(ex.message(), this->val->exprloc, ex);
                }
            }
        } else {
            ret = this->rettype->GetType(ctx);
        }
        evaluatingtype = false;
        assert(ret != NULL);
        ctx.PopLocalScope();
        this->functype = Ides::Types::FunctionType::Get(ret, argTypes);
        return this->functype;
    }
    
    llvm::Value* ASTFunction::GetValue(ParseContext& ctx)
    {
        if (func == NULL) {
            llvm::FunctionType *FT = static_cast<llvm::FunctionType*>(this->GetType(ctx)->GetLLVMType(ctx));
            func = llvm::Function::Create(FT, llvm::GlobalValue::ExternalLinkage, this->GetMangledName());
            func->setGC("refcount");
        }
        return func;
    }
    
    void ASTFunction::GenBody(ParseContext& ctx)
    {
        assert(func != NULL && (this->val != NULL || this->body != NULL));
        
        ctx.PushLocalScope();
        ctx.PushFunction(this);
        
        auto i = args->begin();
        llvm::Function::arg_iterator ai = func->arg_begin();
        for (; i != args->end() && ai != func->arg_end(); ++ai, ++i) {
            ASTDeclaration* decl = dynamic_cast<ASTDeclaration*>(*i);
            decl->val = ai;
            ai->setName(decl->name->name);
            ctx.GetLocalSymbols()->insert(std::make_pair(decl->name->name, decl));
        }
        assert (i == args->end() && ai == func->arg_end());
        
        llvm::BasicBlock* entryblock = llvm::BasicBlock::Create(ctx.GetIRBuilder()->getContext(), "entry", this->func);
        //this->retblock = llvm::BasicBlock::Create(ctx.GetIRBuilder()->getContext(), "return", this->func);
        
        ctx.GetIRBuilder()->SetInsertPoint(entryblock);
        try {
            if (this->val) {
                if (this->functype->retType->IsEquivalentType(Ides::Types::VoidType::GetSingletonPtr())
                    || this->functype->retType->IsEquivalentType(Ides::Types::UnitType::GetSingletonPtr())) {
                    this->val->GetValue(ctx);
                    ctx.GetIRBuilder()->CreateRetVoid();
                } else {
                    ctx.GetIRBuilder()->CreateRet(this->val->GetValue(ctx));
                }
            }
            else if (this->body) {
                this->body->GetValue(ctx);
            }
            llvm::verifyFunction(*func);
            func->dump();
        } catch (const Ides::Diagnostics::CompileError& ex) {
            ctx.PopFunction();
            ctx.PopLocalScope();
            throw Ides::Diagnostics::CompileError(ex.message(), this->exprloc, ex);
        }
        
        
        ctx.PopFunction();
        ctx.PopLocalScope();
    }
    
    llvm::Value* ASTCompoundStatement::GetValue(ParseContext &ctx) {
        ctx.PushLocalScope();
        
        llvm::BasicBlock* scope = llvm::BasicBlock::Create(ctx.GetIRBuilder()->getContext(), "scope", (llvm::Function*)ctx.GetEvaluatingFunction()->GetValue(ctx));
        ctx.GetIRBuilder()->CreateBr(scope);
        ctx.GetIRBuilder()->SetInsertPoint(scope);
        for (auto i = this->begin(); i != this->end(); ++i) {
            try {
                (*i)->GetValue(ctx);
            } catch (const Ides::AST::UnitValueException& ex) {
                break;
            }
        }
        
        ctx.PopLocalScope();
        return NULL;
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
                    err << "expecting argument of type '" << (*defi)->ToString() << "' at: " << this->exprloc.GetText();
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
    
    llvm::Value* ASTReturnExpression::GetValue(ParseContext& ctx) {
        const Ides::Types::Type* exprtype = this->retval->GetType(ctx);
        const Ides::Types::Type* funcrettype = ctx.GetEvaluatingFunction()->rettype->GetType(ctx);
        if (funcrettype == Ides::Types::VoidType::GetSingletonPtr()) {
            if (this->retval != NULL)
                throw Ides::Diagnostics::CompileError("unexpected expression in return from a function with void return type", retval->exprloc);
            ctx.GetIRBuilder()->CreateRetVoid();
        }
        else if (funcrettype == Ides::Types::UnitType::GetSingletonPtr()) {
            throw Ides::Diagnostics::CompileError("functions with unit return type cannot return", this->exprloc);
        }
        else if (this->retval == NULL && funcrettype == Ides::Types::VoidType::GetSingletonPtr()) {
            throw Ides::Diagnostics::CompileError("expected expression", this->exprloc);
        }
        else if (exprtype->IsEquivalentType(funcrettype)) {
            ctx.GetIRBuilder()->CreateRet(this->retval->GetValue(ctx));
        }
        else {
            ctx.GetIRBuilder()->CreateRet(this->retval->GetType(ctx)->Convert(ctx, this->retval->GetValue(ctx), funcrettype));
        }
        throw Ides::AST::UnitValueException();
    }
    
    const Ides::Types::Type* ASTReturnExpression::GetType(ParseContext& ctx) {
        return Ides::Types::UnitType::GetSingletonPtr();
    }
    
    const Ides::Types::Type* ASTInfixExpression::GetType(ParseContext& ctx) {
        // Handle numeric types
        if (const Ides::Types::NumberType* lhsnumtype = dynamic_cast<const Ides::Types::NumberType*>(this->lhs->GetType(ctx))) {
            try {
                return lhsnumtype->GetOperatorType(this->func->name, ctx, lhs, rhs);
            } catch (const Ides::Diagnostics::CompileError& e) {
                throw Ides::Diagnostics::CompileError(e.message(), this->exprloc, e);
            }
        }
        return ASTExpression::GetType(ctx);
        
    }
    
    const Ides::Types::Type* ASTTypeName::GetType(ParseContext& ctx) {
        throw Ides::Diagnostics::CompileError("no such type " + this->name->name, this->exprloc);
    }
    
    llvm::Value* ASTInfixExpression::GetValue(ParseContext& ctx) {
        // Handle numeric types
        if (const Ides::Types::NumberType* lhsnumtype = dynamic_cast<const Ides::Types::NumberType*>(this->lhs->GetType(ctx))) {
            try {
                return lhsnumtype->GetOperatorValue(this->func->name, ctx, lhs, rhs);
            } catch (const Ides::Diagnostics::CompileError& e) {
                throw Ides::Diagnostics::CompileError(e.message(), this->exprloc, e);
            }
        }
        return ASTExpression::GetValue(ctx);
    }
    
}
}