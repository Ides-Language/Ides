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
    
    
    llvm::Value* ASTExpression::GetPointerValue(ParseContext& ctx) {
        llvm::Value* ptr = ctx.GetIRBuilder()->CreateAlloca(this->GetType(ctx)->GetLLVMType(ctx));
        ctx.GetIRBuilder()->CreateStore(this->GetValue(ctx), ptr);
        return ptr;
    }
    
    void ASTCompilationUnit::Compile(ParseContext& ctx) {
        
        ParseContext::ScopedLocalScope localScope(ctx);
        
        llvm::NamedMDNode* node = ctx.GetModule()->getOrInsertNamedMetadata("ides");
        
        for (auto i = this->begin(); i != this->end(); ++i) {
            if (ASTFunction* f = dynamic_cast<ASTFunction*>(*i)) {
                ctx.GetPublicSymbols()->insert(std::make_pair(f->GetMangledName(), f));
            } else if (ASTStruct* s = dynamic_cast<ASTStruct*>(*i)) {
                ctx.GetPublicSymbols()->insert(std::make_pair(s->GetName(), s));
            }
        }
        
        std::queue<ASTFunction*> functions;
        std::queue<ASTStruct*> structs;
        for (auto i = this->begin(); i != this->end(); ++i) {
            try {
                (*i)->GetValue(ctx);
                
                if (ASTFunction* func = dynamic_cast<ASTFunction*>(*i)) {
                    functions.push(func);
                } else if (ASTStruct* str = dynamic_cast<ASTStruct*>(*i)) {
                    structs.push(str);
                }
            } catch (const Ides::Diagnostics::CompileIssue& ex) {
                ctx.Issue(ex);
            }
        }
        
        while (!structs.empty()) {
            try {
                node->addOperand((llvm::MDNode*)structs.front()->GetMDNode(ctx));
                structs.front()->GenType(ctx);
            } catch (const Ides::Diagnostics::CompileIssue& ex) {
                ctx.Issue(ex);
            }
            structs.pop();
        }
        
        while (!functions.empty()) {
            try {
                node->addOperand((llvm::MDNode*)functions.front()->GetMDNode(ctx));
                if (functions.front()->body || functions.front()->val)
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
            func->setGC("shadow-stack");
            
            if (this->GetType(ctx)->IsEquivalentType(Ides::Types::UnitType::GetSingletonPtr())) {
                func->addFnAttr(llvm::Attribute::NoReturn);
            }
        }
        return func;
    }
    
    void ASTFunction::GenBody(ParseContext& ctx)
    {
        assert(func != NULL && (this->val != NULL || this->body != NULL));
        
        ParseContext::ScopedLocalScope localScope(ctx);
        ParseContext::ScopedFunction thisFunction(ctx, this);
        
        llvm::BasicBlock* entryblock = llvm::BasicBlock::Create(ctx.GetIRBuilder()->getContext(), "entry", this->func);
        
        ctx.GetIRBuilder()->SetInsertPoint(entryblock);
        
        if (args) {
            auto i = args->begin();
            llvm::Function::arg_iterator ai = func->arg_begin();
            for (; i != args->end() && ai != func->arg_end(); ++ai, ++i) {
                ASTDeclaration* decl = dynamic_cast<ASTDeclaration*>(*i);
                if (decl->vartype == ASTDeclaration::DECL_VAR) {
                    decl->GetValue(ctx);
                    ctx.GetIRBuilder()->CreateStore(ai, decl->val);
                }
                ai->setName(decl->name->name);
                ctx.GetLocalSymbols()->insert(std::make_pair(decl->name->name, decl));
            }
            assert (i == args->end() && ai == func->arg_end());
        }
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
                            ctx.GetIRBuilder()->CreateRet(val->GetValue(ctx, rettype));

                        } catch (const std::runtime_error& ex) {
                            throw Ides::Diagnostics::CompileError(ex.what(), this->val->exprloc);
                        }
                    } else {
                        throw Ides::Diagnostics::CompileError("returning expression of type " + valtype->ToString() + " from a function with return type " + rettype->ToString(), this->val->exprloc);
                    }
                }
            }
            else if (this->body) {
                try {
                    this->body->GetValue(ctx);
                    if (this->GetReturnType(ctx)->IsEquivalentType(Ides::Types::VoidType::GetSingletonPtr())) {
                        // Function didn't return, but it's void, so NBD.
                        ctx.GetIRBuilder()->CreateRetVoid();
                    } else {
                        throw Ides::Diagnostics::CompileError("control reaches end of non-void function", this->exprloc);
                    }
                } catch (const Ides::AST::UnitValueException& ex) {
                    // Function returned.
                }
            }
            llvm::verifyFunction(*func);
            //func->dump();
        } catch (const Ides::Diagnostics::CompileError& ex) {
            throw Ides::Diagnostics::CompileError(ex.message(), this->exprloc, ex);
        }
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
    
    llvm::Value* ASTDeclaration::GetPointerValue(ParseContext& ctx) {
        if (this->val == NULL) {
            ctx.GetLocalSymbols()->insert(std::make_pair(this->name->name, this));
            
            this->val = ctx.GetIRBuilder()->CreateAlloca(this->GetType(ctx)->GetLLVMType(ctx), 0, this->name->name);
            if (this->initval != NULL) {
                llvm::Value* iv = this->initval->GetValue(ctx);
                ctx.GetIRBuilder()->CreateStore(iv, this->val);
            }
        }
        return this->val;
    }
    
    llvm::Value* ASTDeclaration::GetValue(ParseContext& ctx, const Ides::Types::Type* to)
    {
        return this->GetType(ctx)->Convert(ctx, this->GetValue(ctx), to);
    }
    
    llvm::Value* ASTDeclaration::GetValue(ParseContext& ctx)
    {
        return ctx.GetIRBuilder()->CreateLoad(this->GetPointerValue(ctx), this->name->name + "_res");
    }
    
    const Ides::Types::Type* ASTDeclaration::GetType(ParseContext &ctx) {
        const Ides::Types::Type* ret = this->type ? this->type->GetType(ctx) : this->initval->GetType(ctx);
        return ret;
    }
    
    AST* ASTIdentifier::GetDeclaration(ParseContext& ctx) {
        AST* ret = ctx.GetLocalSymbols()->LookupRecursive(this->name);
        if (ret == NULL) throw Ides::Diagnostics::CompileError("no such identifier " + this->name, this->exprloc);
        return ret;
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
        AST* t = ctx.GetLocalSymbols()->LookupRecursive(this->name->name);
        if (t != NULL) return t->GetType(ctx);
        throw Ides::Diagnostics::CompileError("no such type " + this->name->name, this->exprloc);
    }
    
    
}
}