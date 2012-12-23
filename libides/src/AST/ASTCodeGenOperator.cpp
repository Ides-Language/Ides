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
    
    
    const Ides::Types::Type* ASTReturnExpression::GetType(ParseContext& ctx) {
        return Ides::Types::UnitType::GetSingletonPtr();
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
    
    
    
    const Ides::Types::Type* ASTCastExpression::GetType(ParseContext &ctx) {
        return this->rhs->GetType(ctx);
    }
    
    llvm::Value* ASTCastExpression::GetValue(ParseContext &ctx) {
        const Ides::Types::Type* rhstype = rhs->GetType(ctx);
        const Ides::Types::Type* lhstype = lhs->GetType(ctx);
        if (const Ides::Types::NumberType* lhsnumtype = dynamic_cast<const Ides::Types::NumberType*>(lhstype)) {
            const Ides::Types::NumberType* rhsnumtype = dynamic_cast<const Ides::Types::NumberType*>(rhstype);
            switch (lhsnumtype->GetNumberClass()) {
                case Ides::Types::NumberType::N_SINT:
                    switch (rhsnumtype->GetNumberClass()) {
                        case Ides::Types::NumberType::N_SINT:
                        case Ides::Types::NumberType::N_UINT:
                            return ctx.GetIRBuilder()->CreateIntCast(lhs->GetValue(ctx), rhsnumtype->GetLLVMType(ctx), rhsnumtype->GetNumberClass() == Ides::Types::NumberType::N_SINT);
                        case Ides::Types::NumberType::N_FLOAT:
                            return ctx.GetIRBuilder()->CreateSIToFP(lhs->GetValue(ctx), rhsnumtype->GetLLVMType(ctx));
                    }
                case Ides::Types::NumberType::N_UINT:
                    switch (rhsnumtype->GetNumberClass()) {
                        case Ides::Types::NumberType::N_SINT:
                        case Ides::Types::NumberType::N_UINT:
                            return ctx.GetIRBuilder()->CreateIntCast(lhs->GetValue(ctx), rhsnumtype->GetLLVMType(ctx), rhsnumtype->GetNumberClass() == Ides::Types::NumberType::N_SINT);
                        case Ides::Types::NumberType::N_FLOAT:
                            return ctx.GetIRBuilder()->CreateUIToFP(lhs->GetValue(ctx), rhsnumtype->GetLLVMType(ctx));
                    }
                case Ides::Types::NumberType::N_FLOAT:
                    switch (rhsnumtype->GetNumberClass()) {
                        case Ides::Types::NumberType::N_SINT:
                            return ctx.GetIRBuilder()->CreateFPToSI(lhs->GetValue(ctx), rhsnumtype->GetLLVMType(ctx));
                        case Ides::Types::NumberType::N_UINT:
                            return ctx.GetIRBuilder()->CreateFPToUI(lhs->GetValue(ctx), rhsnumtype->GetLLVMType(ctx));
                        case Ides::Types::NumberType::N_FLOAT:
                            return ctx.GetIRBuilder()->CreateFPCast(lhs->GetValue(ctx), rhsnumtype->GetLLVMType(ctx));
                    }
            }
        }
        throw Ides::Diagnostics::CompileError("could not cast expression to type " + rhstype->ToString(), lhs->exprloc + rhs->exprloc);
    }
    
    
    
    const Ides::Types::Type* ASTInfixExpression::GetType(ParseContext& ctx) {
        // Handle numeric types
        if (const Ides::Types::NumberType* lhsnumtype = dynamic_cast<const Ides::Types::NumberType*>(this->lhs->GetType(ctx))) {
            return lhsnumtype->GetOperatorType(this->func->name, ctx, lhs, rhs);
        }
        return ASTExpression::GetType(ctx);
        
    }
    
    llvm::Value* ASTInfixExpression::GetValue(ParseContext& ctx) {
        // Handle numeric types
        if (const Ides::Types::NumberType* lhsnumtype = dynamic_cast<const Ides::Types::NumberType*>(this->lhs->GetType(ctx))) {
            return lhsnumtype->GetOperatorValue(this->func->name, ctx, lhs, rhs);
        }
        return ASTExpression::GetValue(ctx);
    }
    
    
    
    const Ides::Types::Type* ASTAssignmentExpression::GetType(ParseContext& ctx) {
        if (ASTIdentifier* ident = dynamic_cast<ASTIdentifier*>(this->lhs)) {
            AST* variable = ctx.GetLocalSymbols()->LookupRecursive(ident->name);
            if (variable == NULL) throw Ides::Diagnostics::CompileError("no such identifier " + ident->name, this->exprloc);
            if (ASTDeclaration* decl = dynamic_cast<ASTDeclaration*>(variable)) {
                return decl->GetType(ctx);
            }
            throw Ides::Diagnostics::CompileError("variable required in lhs argument of assignment", this->exprloc);
        }
        throw Ides::Diagnostics::CompileError("lhs of assignment must be an identifier", this->exprloc);
    }
    
    llvm::Value* ASTAssignmentExpression::GetValue(ParseContext& ctx) {
        if (ASTIdentifier* ident = dynamic_cast<ASTIdentifier*>(this->lhs)) {
            AST* variable = ctx.GetLocalSymbols()->LookupRecursive(ident->name);
            if (variable == NULL) throw Ides::Diagnostics::CompileError("no such identifier " + ident->name, this->exprloc);
            if (ASTDeclaration* decl = dynamic_cast<ASTDeclaration*>(variable)) {
                if (decl->vartype == ASTDeclaration::DECL_VAL) throw Ides::Diagnostics::CompileError("cannot reassign val", this->exprloc);
                llvm::Value* newval = rhs->GetType(ctx)->Convert(ctx, rhs->GetValue(ctx), lhs->GetType(ctx));
                ctx.GetIRBuilder()->CreateStore(newval, decl->val);
                return newval;
            }
            throw Ides::Diagnostics::CompileError("variable required in lhs argument of assignment", this->exprloc);
        }
        throw Ides::Diagnostics::CompileError("lhs of assignment must be an identifier", this->exprloc);
    }

    
}
}