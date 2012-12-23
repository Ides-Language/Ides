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
                ctx.GetIRBuilder()->CreateRet(this->retval->GetValue(ctx, funcrettype));
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
        return this->lhs->GetType(ctx);
    }
    
    llvm::Value* ASTAssignmentExpression::GetValue(ParseContext& ctx) {
        llvm::Value* newval = rhs->GetValue(ctx); //rhs->GetType(ctx)->Convert(ctx, rhs->GetValue(ctx), this->lhs->GetType(ctx));
        llvm::Value* target = lhs->GetPointerValue(ctx);
        ctx.GetIRBuilder()->CreateStore(newval, target);
        return target;
    }

    const Ides::Types::Type* ASTAddressOfExpression::GetType(ParseContext &ctx) {
        return arg->GetType(ctx)->PtrType();
    }
    
    llvm::Value* ASTAddressOfExpression::GetValue(ParseContext &ctx) {
        return ctx.GetIRBuilder()->CreateLoad(this->GetPointerValue(ctx));
    }
    
    llvm::Value* ASTAddressOfExpression::GetPointerValue(ParseContext &ctx) {
        llvm::Value* argval = arg->GetPointerValue(ctx);
        llvm::Value* ptrval = ctx.GetIRBuilder()->CreateAlloca(this->GetType(ctx)->GetLLVMType(ctx));
        ctx.GetIRBuilder()->CreateStore(argval, ptrval);
        return ptrval;
    }
    
    const Ides::Types::Type* ASTDereferenceExpression::GetType(ParseContext &ctx) {
        const Ides::Types::PointerType* ptr = dynamic_cast<const Ides::Types::PointerType*>(arg->GetType(ctx));
        if (ptr == NULL) throw Ides::Diagnostics::CompileError("cannot dereference non-pointer type", this->exprloc);
        return ptr->GetTargetType();
    }
    
    llvm::Value* ASTDereferenceExpression::GetValue(ParseContext &ctx) {
        return ctx.GetIRBuilder()->CreateLoad(this->GetPointerValue(ctx), "deref");
    }
    
    llvm::Value* ASTDereferenceExpression::GetPointerValue(ParseContext &ctx) {
        return ctx.GetIRBuilder()->CreateLoad(arg->GetPointerValue(ctx), "deref");
    }
}
}