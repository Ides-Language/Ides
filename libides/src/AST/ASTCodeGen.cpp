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
            try {
                (*i)->GetValue(ctx);
                
                if (ASTFunction* func = dynamic_cast<ASTFunction*>(*i)) {
                    if (func->body || func->val)
                        functions.push(func);
                }
            } catch (const Ides::Diagnostics::CompileIssue& ex) {
                ctx.Issue(ex);
            }
        }
        
        while (!functions.empty()) {
            try {
                functions.front()->GenBody(ctx);
            } catch (const Ides::Diagnostics::CompileIssue& ex) {
                ctx.Issue(ex);
            }
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
        
        ParseContext::ScopedLocalScope localScope(ctx);
        
        std::vector<const Ides::Types::Type*> argTypes;
        if (this->args) {
            for (auto i = this->args->begin(); i != this->args->end(); ++i) {
                ASTDeclaration* decl = dynamic_cast<ASTDeclaration*>(*i);
                ctx.GetLocalSymbols()->insert(std::make_pair(decl->name->name, decl));
                argTypes.push_back((*i)->GetType(ctx));
            }
        }
        
        const Ides::Types::Type* ret = NULL;
        
        if (this->returntype == NULL) {
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
            ret = this->returntype->GetType(ctx);
        }
        
        evaluatingtype = false;
        assert(ret != NULL);
        this->functype = Ides::Types::FunctionType::Get(ret, argTypes);
        return this->functype;
    }
    
    const Ides::Types::Type* ASTFunction::GetReturnType(ParseContext& ctx) {

        if (functype == NULL) this->GetType(ctx);
        return functype->retType;
    }
    
    llvm::Value* ASTFunction::GetValue(ParseContext& ctx)
    {
        if (func == NULL) {
            llvm::FunctionType *FT = static_cast<llvm::FunctionType*>(this->GetType(ctx)->GetLLVMType(ctx));
            //func = (llvm::Function*)ctx.GetModule()->getOrInsertFunction(this->GetMangledName(), FT);
            func = llvm::Function::Create(FT, llvm::GlobalValue::ExternalLinkage, this->GetMangledName(), ctx.GetModule());
            //func->setGC("refcount");
        }
        return func;
    }
    
    void ASTFunction::GenBody(ParseContext& ctx)
    {
        assert(func != NULL && (this->val != NULL || this->body != NULL));
        
        ParseContext::ScopedLocalScope localScope(ctx);
        ParseContext::ScopedFunction thisFunction(ctx, this);
        
        if (args) {
            auto i = args->begin();
            llvm::Function::arg_iterator ai = func->arg_begin();
            for (; i != args->end() && ai != func->arg_end(); ++ai, ++i) {
                ASTDeclaration* decl = dynamic_cast<ASTDeclaration*>(*i);
                decl->val = ai;
                ai->setName(decl->name->name);
                ctx.GetLocalSymbols()->insert(std::make_pair(decl->name->name, decl));
            }
            assert (i == args->end() && ai == func->arg_end());
        }
        
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
                    const Ides::Types::Type* valtype = this->val->GetType(ctx);
                    const Ides::Types::Type* rettype = this->GetReturnType(ctx);
                    if (valtype->IsEquivalentType(rettype)) {
                        ctx.GetIRBuilder()->CreateRet(this->val->GetValue(ctx));
                    } else if (valtype->HasImplicitConversionTo(rettype)) {
                        try {
                            ctx.GetIRBuilder()->CreateRet(valtype->Convert(ctx, this->val->GetValue(ctx), rettype));
                        } catch (const std::runtime_error& ex) {
                            throw Ides::Diagnostics::CompileError(ex.what(), this->val->exprloc);
                        }
                    } else {
                        throw Ides::Diagnostics::CompileError("returning expression of type " + valtype->ToString() + " from a function with return type " + rettype->ToString(), this->val->exprloc);
                    }
                }
            }
            else if (this->body) {
                this->body->GetValue(ctx);
            }
            llvm::verifyFunction(*func);
            //func->dump();
        } catch (const Ides::Diagnostics::CompileError& ex) {
            throw Ides::Diagnostics::CompileError(ex.message(), this->exprloc, ex);
        }
    }
    
    llvm::Value* ASTCompoundStatement::GetValue(ParseContext &ctx) {
        
        ParseContext::ScopedLocalScope localScope(ctx);
        
        llvm::BasicBlock* scope = llvm::BasicBlock::Create(ctx.GetIRBuilder()->getContext(), "scope", (llvm::Function*)ctx.GetEvaluatingFunction()->GetValue(ctx));
        ctx.GetIRBuilder()->CreateBr(scope);
        ctx.GetIRBuilder()->SetInsertPoint(scope);
        for (auto i = this->begin(); i != this->end(); ++i) {
            try {
                (*i)->GetValue(ctx);
            } catch (const Ides::AST::UnitValueException& ex) {
                if (++i != this->end()) {
                    ctx.Issue(Ides::Diagnostics::CompileWarning("unreachable code", (*i)->exprloc));
                }
                break;
            }
        }
        
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
        const Ides::Types::Type* exprtype = this->retval ? this->retval->GetType(ctx) : Ides::Types::VoidType::GetSingletonPtr();
        const Ides::Types::Type* funcrettype = ctx.GetEvaluatingFunction()->GetReturnType(ctx);
        if (funcrettype == Ides::Types::VoidType::GetSingletonPtr()) {
            if (this->retval != NULL)
                throw Ides::Diagnostics::CompileError("returning an expression from a function with void return type", this->exprloc);
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
            try {
                ctx.GetIRBuilder()->CreateRet(this->retval->GetType(ctx)->Convert(ctx, this->retval->GetValue(ctx), funcrettype));
            } catch (const std::exception& ex) {
                throw Ides::Diagnostics::CompileError(ex.what(), this->exprloc);
            }
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
    
    const Ides::Types::Type* ASTFunctionType::GetType(ParseContext &ctx) {
        std::vector<const Ides::Types::Type*> argTypes;
        const Ides::Types::Type* rtype = this->rettype ? this->rettype->GetType(ctx) : Ides::Types::VoidType::GetSingletonPtr();
        if (this->argtypes) {
            for (auto i = this->argtypes->begin(); i != this->argtypes->end(); ++i) {
                argTypes.push_back((*i)->GetType(ctx));
            }
        }
        return Ides::Types::FunctionType::Get(rtype, argTypes);
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