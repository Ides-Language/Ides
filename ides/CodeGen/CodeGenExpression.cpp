//
//  CodeGenExpression.cpp
//  ides
//
//  Created by Sean Edwards on 1/2/13.
//
//

#include "CodeGen.h"
#include <ides/Diagnostics/Diagnostics.h>

#include <ides/AST/ConstantExpression.h>
#include <ides/AST/Expression.h>
#include <llvm/BasicBlock.h>


namespace Ides {
namespace CodeGen {
    
    
    using namespace Ides::Diagnostics;
    
    
    void CodeGen::Visit(Ides::AST::AssignmentExpression* ast) { SETTRACE("CodeGen::Visit(AssignmentExpression)")
        llvm::Value* toExpr = GetPtr(ast->GetLHS());
        llvm::Value* fromExpr = GetValue(ast->GetRHS(), ast->GetLHS()->GetType(actx));
        auto inst = builder->CreateStore(fromExpr, toExpr);
        if (this->dibuilder) {
            inst->setDebugLoc(this->GetDebugLoc(ast));
            
            auto i = this->dbgvalues.find(toExpr);
            if (i != this->dbgvalues.end()) {
                dibuilder->insertDbgValueIntrinsic(toExpr,
                                                   0,
                                                   llvm::DIVariable(i->second),
                                                   builder->GetInsertBlock());
            }
        }
        last = toExpr;
    }
    
    void CodeGen::Visit(Ides::AST::IdentifierExpression* ast) { SETTRACE("CodeGen::Visit(IdentifierExpression)")
        Ides::AST::Declaration* decl = actx.GetCurrentScope()->GetMember(actx, ast->GetName());
        if (decl) {
            decl->Accept(this);
            return;
        }
        
        throw detail::CodeGenError(*diag, UNKNOWN_IDENTIFIER, ast->exprloc) << ast->GetName();
    }
    
    void CodeGen::Visit(Ides::AST::ReturnExpression* ast) { SETTRACE("CodeGen::Visit(ReturnExpression)")
        const Ides::Types::Type* exprtype = ast->GetRetType(actx);
        const Ides::Types::Type* funcrettype = this->currentFunctions.top()->GetReturnType(actx);
        
        if (funcrettype == Ides::Types::VoidType::GetSingletonPtr()) {
            if (ast->GetRetVal() != NULL) {
                throw detail::CodeGenError(*diag, RETURN_FROM_VOID, ast->exprloc);
            }
            last = builder->CreateRetVoid();
        }
        else if (funcrettype == Ides::Types::UnitType::GetSingletonPtr()) {
            throw detail::CodeGenError(*diag, RETURN_FROM_UNIT, ast->exprloc);
        }
        else if (ast->GetRetVal() == NULL && funcrettype != Ides::Types::VoidType::GetSingletonPtr()) {
            throw detail::CodeGenError(*diag, RETURN_NO_EXPRESSION, ast->exprloc);
        }
        else if (exprtype->IsEquivalentType(funcrettype)) {
            llvm::Value* retVal = GetValue(ast->GetRetVal());
            last = builder->CreateRet(retVal);
        }
        else {
            // Returning from function with return type.
            last = builder->CreateRet(GetValue(ast->GetRetVal(), funcrettype));
        }
        if (dibuilder) llvm::cast<llvm::Instruction>(last)->setDebugLoc(GetDebugLoc(ast));
        throw detail::UnitValueException();
    }
    
    
    void CodeGen::Visit(Ides::AST::DotExpression* ast) { SETTRACE("CodeGen::Visit(DotExpression)")
        const Ides::Types::Type* exprtype = ast->GetExpression().GetType(actx);
        
        llvm::Value* ptr = GetPtr(&ast->GetExpression());
        
        if (const Ides::Types::StructType* st = dynamic_cast<const Ides::Types::StructType*>(exprtype)) {
            int memberidx = st->GetMemberIndex(*ast->GetToken());
            if (memberidx == -1) throw detail::CodeGenError(*diag, UNKNOWN_MEMBER, ast->GetToken().exprloc) << exprtype->ToString() << *ast->GetToken();
            
            last = builder->CreateStructGEP(ptr, memberidx, exprtype->ToString() + "." + *ast->GetToken());
            return;
        }
        else {
        }
        this->Diag(Ides::Diagnostics::COMPILER_NOT_IMPLEMENTED, ast);
    }
    
    
    void CodeGen::Visit(Ides::AST::BinaryExpression<OP_RARROW>* ast) {
        const Ides::Types::Type* lhstype = ast->lhs->GetType(actx);
        
        llvm::Value* ptr = GetPtr(ast->lhs.get());
        
        if (const Ides::Types::PointerType* pt = dynamic_cast<const Ides::Types::PointerType*>(lhstype)) {
            if (const Ides::Types::StructType* st = dynamic_cast<const Ides::Types::StructType*>(pt->GetTargetType())) {
                if (Ides::AST::IdentifierExpression* expr = dynamic_cast<Ides::AST::IdentifierExpression*>(ast->rhs.get())) {
                    int memberidx = st->GetMemberIndex(expr->GetName());
                    if (memberidx == -1) throw detail::CodeGenError(*diag, UNKNOWN_MEMBER, ast->rhs->exprloc) << lhstype->ToString(), expr->GetName();
                    
                    ptr = builder->CreateLoad(ptr, "arrowderef");
                    last = builder->CreateStructGEP(ptr, memberidx, lhstype->ToString() + "->" + expr->GetName());
                    return;
                }
            }
        }
        this->Diag(Ides::Diagnostics::COMPILER_NOT_IMPLEMENTED, ast);
    }
    
    void CodeGen::Visit(Ides::AST::BinaryExpression<OP_LARROW>* ast) {
        this->Diag(Ides::Diagnostics::COMPILER_NOT_IMPLEMENTED, ast);
    }
    
    void CodeGen::Visit(Ides::AST::FunctionCallExpression* ast) { SETTRACE("CodeGen::Visit(FunctionCallExpression)")
        const Ides::Types::FunctionType* function = static_cast<const Ides::Types::FunctionType*>(ast->GetFunction()->GetType(actx));
        
        const Ides::AST::ExpressionList& args = ast->GetArgs();
        llvm::Function* func = static_cast<llvm::Function*>(GetPtr(ast->GetFunction()));
        std::vector<llvm::Value*> fnargs;
        auto defi = function->argTypes.begin();
        for (auto i = args.begin(); i != args.end(); ++i) {
            if (defi != function->argTypes.end()) {
                fnargs.push_back(GetValue(*i, *defi));
                ++defi;
            }
            else {
                fnargs.push_back(GetValue(*i));
            }
        }
        
        if (function->argTypes.size() > args.size()) {
            throw detail::CodeGenError(*diag, CALL_INSUFFICIENT_ARGS, ast->exprloc) << (int)function->argTypes.size() << (int)args.size();
        } else if (function->argTypes.size() < args.size() && !function->isVarArgs) {
            throw detail::CodeGenError(*diag, CALL_TOO_MANY_ARGS, ast->exprloc) << (int)function->argTypes.size() << (int)args.size();
        }
        last = builder->CreateCall(func, fnargs);
    }
    
    
    void CodeGen::Visit(Ides::AST::AddressOfExpression* ast) { SETTRACE("CodeGen::Visit(AddressOfExpression)")
        const Ides::Types::Type* exprType = ast->GetType(actx);
        llvm::AllocaInst* ptr = builder->CreateAlloca(GetLLVMType(exprType), 0, "addrof");
        ptr->setAlignment(exprType->GetAlignment());
        builder->CreateStore(GetPtr(ast->arg.get()), ptr);
        last = ptr;
    }
    
    void CodeGen::Visit(Ides::AST::DereferenceExpression* ast) { SETTRACE("CodeGen::Visit(DereferenceExpression)")
        const Ides::Types::Type* astType = ast->arg->GetType(actx);
        if (dynamic_cast<const Ides::Types::PointerType*>(astType)) {
            last = builder->CreateLoad(this->GetPtr(ast->arg.get()), "deref");
            return;
        }
        throw detail::CodeGenError(*diag, OP_NO_SUCH_OPERATOR, ast->exprloc) << "*" << astType->ToString();
    }
    
    void CodeGen::Visit(Ides::AST::CastExpression* ast) {
        last = Cast(ast->lhs.get(), ast->GetType(actx));
    }
    
    void CodeGen::Visit(Ides::AST::UnaryExpression<OP_INC>* ast) { SETTRACE("CodeGen::Visit(UnaryExpression<OP_INC>)")
        const Ides::Types::Type* exprType = ast->GetType(actx);
        llvm::Value* ptr = this->GetPtr(ast->arg.get());
        llvm::Value* oldVal = builder->CreateLoad(ptr);
        llvm::Value* newVal = builder->CreateAdd(oldVal, llvm::ConstantInt::get(GetLLVMType(exprType), 1));
        builder->CreateStore(newVal, ptr);
        
        if (ast->type == Ides::AST::UnaryExpression<OP_INC>::UNARY_POSTFIX)
            last = oldVal;
        else
            last = newVal;
    
    }
    
    void CodeGen::Visit(Ides::AST::UnaryExpression<OP_DEC>* ast) { SETTRACE("CodeGen::Visit(UnaryExpression<OP_DEC>)")
        const Ides::Types::Type* exprType = ast->GetType(actx);
        llvm::Value* ptr = this->GetPtr(ast->arg.get());
        llvm::Value* oldVal = builder->CreateLoad(ptr);
        llvm::Value* newVal = builder->CreateSub(oldVal, llvm::ConstantInt::get(GetLLVMType(exprType), 1));
        builder->CreateStore(newVal, ptr);
        
        last = (ast->type == Ides::AST::UnaryExpression<OP_DEC>::UNARY_POSTFIX) ? oldVal : newVal;
    }
    
    void CodeGen::Visit(Ides::AST::UnaryExpression<OP_NOT>* ast) { SETTRACE("CodeGen::Visit(UnaryExpression<OP_NOT>)")
        llvm::Value* expr = this->GetValue(ast->arg.get(), Ides::Types::Integer1Type::GetSingletonPtr());
        last = builder->CreateNot(expr);
    }
    
    void CodeGen::Visit(Ides::AST::UnaryExpression<OP_MINUS>* ast) { SETTRACE("CodeGen::Visit(UnaryExpression<OP_MINUS>)")
        this->Diag(Ides::Diagnostics::COMPILER_NOT_IMPLEMENTED, ast);
    }
    
    void CodeGen::Visit(Ides::AST::UnaryExpression<OP_BNOT>* ast) { SETTRACE("CodeGen::Visit(UnaryExpression<OP_BNOT>)")
        llvm::Value* expr = this->GetValue(ast->arg.get(), Ides::Types::Integer1Type::GetSingletonPtr());
        last = builder->CreateNot(expr);
    }
    
    
#define CREATE_BINARY_EXPRESSION(op, generator, optxt) \
    void CodeGen::Visit(Ides::AST::BinaryExpression<op>* ast) { SETTRACE("CodeGen::Visit(BinaryExpression<" #op ">)") \
        const Ides::Types::Type* resultType = ast->GetType(actx); \
        if (resultType->IsNumericType()) { \
            llvm::Value* lhsresult = Cast(ast->lhs.get(), resultType); \
            llvm::Value* rhsresult = Cast(ast->rhs.get(), resultType); \
            last = builder->generator(lhsresult, rhsresult, #op); \
            return; \
        } \
        throw detail::CodeGenError(*diag, OP_NO_SUCH_OPERATOR, ast->exprloc) << optxt << resultType->ToString(); \
    }
    
    CREATE_BINARY_EXPRESSION(OP_PLUS, CreateAdd, "+")
    CREATE_BINARY_EXPRESSION(OP_MINUS, CreateSub, "-")
    CREATE_BINARY_EXPRESSION(OP_STAR, CreateMul, "*")
    
    void CodeGen::Visit(Ides::AST::BinaryExpression<OP_SLASH>* ast) { SETTRACE("CodeGen::Visit(BinaryExpression<OP_SLASH>)")
        const Ides::Types::Type* resultType = ast->GetType(actx);
        
        if (resultType->IsNumericType()) {
            auto nt = static_cast<const Ides::Types::NumberType*>(resultType);
            llvm::Value* lhsresult = Cast(ast->lhs.get(), resultType);
            llvm::Value* rhsresult = Cast(ast->rhs.get(), resultType);
            
            if (nt->IsSigned())
                last = builder->CreateSDiv(lhsresult, rhsresult);
            else
                last = builder->CreateUDiv(lhsresult, rhsresult);
            return;
        }
    }
    
    void CodeGen::Visit(Ides::AST::BinaryExpression<OP_MOD>* ast) { SETTRACE("CodeGen::Visit(BinaryExpression<OP_MOD>)")
        const Ides::Types::Type* resultType = ast->GetType(actx);
        
        if (resultType->IsNumericType()) {
            auto nt = static_cast<const Ides::Types::NumberType*>(resultType);
            llvm::Value* lhsresult = Cast(ast->lhs.get(), resultType);
            llvm::Value* rhsresult = Cast(ast->rhs.get(), resultType);
            
            if (nt->IsSigned())
                last = builder->CreateSRem(lhsresult, rhsresult);
            else
                last = builder->CreateURem(lhsresult, rhsresult);
            return;
        }
    }
    
    CREATE_BINARY_EXPRESSION(OP_BAND, CreateAnd, "&");
    CREATE_BINARY_EXPRESSION(OP_BOR, CreateOr, "|");
    CREATE_BINARY_EXPRESSION(OP_BXOR, CreateXor, "^");
    
    CREATE_BINARY_EXPRESSION(OP_AND, CreateAnd, "&&");
    CREATE_BINARY_EXPRESSION(OP_OR, CreateOr, "||");
    
    void CodeGen::Visit(Ides::AST::BinaryExpression<OP_ASHL>* ast) { SETTRACE("CodeGen::Visit(BinaryExpression<OP_ASHL>)")
        const Ides::Types::Type* resultType = ast->GetType(actx);
        if (resultType->IsNumericType()) {
            llvm::Value* lhsresult = Cast(ast->lhs.get(), resultType);
            llvm::Value* rhsresult = Cast(ast->rhs.get(), Ides::Types::UInteger64Type::GetSingletonPtr());
            last = builder->CreateShl(lhsresult, rhsresult, "OP_ASHL");
            return;
        }
    }
    void CodeGen::Visit(Ides::AST::BinaryExpression<OP_LSHL>* ast) { SETTRACE("CodeGen::Visit(BinaryExpression<OP_LSHL>)")
        const Ides::Types::Type* resultType = ast->GetType(actx);
        if (resultType->IsNumericType()) {
            llvm::Value* lhsresult = Cast(ast->lhs.get(), resultType);
            llvm::Value* rhsresult = Cast(ast->rhs.get(), Ides::Types::UInteger64Type::GetSingletonPtr());
            last = builder->CreateShl(lhsresult, rhsresult, "OP_LSHL");
            return;
        }
    }
    void CodeGen::Visit(Ides::AST::BinaryExpression<OP_ASHR>* ast) { SETTRACE("CodeGen::Visit(BinaryExpression<OP_ASHR>)")
        const Ides::Types::Type* resultType = ast->GetType(actx);
        if (resultType->IsNumericType()) {
            llvm::Value* lhsresult = Cast(ast->lhs.get(), resultType);
            llvm::Value* rhsresult = Cast(ast->rhs.get(), Ides::Types::UInteger64Type::GetSingletonPtr());
            last = builder->CreateAShr(lhsresult, rhsresult, "OP_ASHR");
            return;
        }
    }
    void CodeGen::Visit(Ides::AST::BinaryExpression<OP_LSHR>* ast) { SETTRACE("CodeGen::Visit(BinaryExpression<OP_LSHR>)")
        const Ides::Types::Type* resultType = ast->GetType(actx);
        if (resultType->IsNumericType()) {
            llvm::Value* lhsresult = Cast(ast->lhs.get(), resultType);
            llvm::Value* rhsresult = Cast(ast->rhs.get(), Ides::Types::UInteger64Type::GetSingletonPtr());
            last = builder->CreateLShr(lhsresult, rhsresult, "OP_LSHR");
            return;
        }
    }
    
    
    void CodeGen::Visit(Ides::AST::BinaryExpression<OP_EQ>* ast) { SETTRACE("CodeGen::Visit(BinaryExpression<OP_EQ>)")
        const Ides::Types::Type* resultType = ast->GetType(actx);
        
        if (resultType->IsNumericType()) {
            auto nt = static_cast<const Ides::Types::NumberType*>(resultType);
            llvm::Value* lhsresult = GetValue(ast->lhs.get());
            llvm::Value* rhsresult = GetValue(ast->rhs.get(), ast->lhs->GetType(actx));
            
            if (nt->IsIntegerType())
                last = builder->CreateICmpEQ(lhsresult, rhsresult, "OP_EQ");
            else
                last = builder->CreateFCmpUEQ(lhsresult, rhsresult, "OP_EQ");
            return;
        }
    }
    void CodeGen::Visit(Ides::AST::BinaryExpression<OP_NE>* ast) { SETTRACE("CodeGen::Visit(BinaryExpression<OP_NE>)")
        const Ides::Types::Type* resultType = ast->GetType(actx);
        
        if (resultType->IsNumericType()) {
            auto nt = static_cast<const Ides::Types::NumberType*>(resultType);
            llvm::Value* lhsresult = GetValue(ast->lhs.get());
            llvm::Value* rhsresult = GetValue(ast->rhs.get(), ast->lhs->GetType(actx));
            
            if (nt->IsIntegerType())
                last = builder->CreateICmpNE(lhsresult, rhsresult, "OP_NE");
            else
                last = builder->CreateFCmpUNE(lhsresult, rhsresult, "OP_NE");
            return;
        }
    }
    void CodeGen::Visit(Ides::AST::BinaryExpression<OP_LT>* ast) { SETTRACE("CodeGen::Visit(BinaryExpression<OP_LT>)")
        const Ides::Types::Type* resultType = ast->GetType(actx);
        
        if (resultType->IsNumericType()) {
            auto nt = static_cast<const Ides::Types::NumberType*>(resultType);
            llvm::Value* lhsresult = GetValue(ast->lhs.get());
            llvm::Value* rhsresult = GetValue(ast->rhs.get(), ast->lhs->GetType(actx));
            
            if (nt->IsIntegerType())
                last = nt->IsSigned() ? builder->CreateICmpSLT(lhsresult, rhsresult, "OP_LT") : builder->CreateICmpULT(lhsresult, rhsresult, "OP_LT");
            else
                last = builder->CreateFCmpULT(lhsresult, rhsresult, "OP_LT");
            return;
        }
    }
    void CodeGen::Visit(Ides::AST::BinaryExpression<OP_LE>* ast) { SETTRACE("CodeGen::Visit(BinaryExpression<OP_LE>)")
        const Ides::Types::Type* resultType = ast->GetType(actx);
        
        if (resultType->IsNumericType()) {
            auto nt = static_cast<const Ides::Types::NumberType*>(resultType);
            llvm::Value* lhsresult = GetValue(ast->lhs.get());
            llvm::Value* rhsresult = GetValue(ast->rhs.get(), ast->lhs->GetType(actx));
            
            if (nt->IsIntegerType())
                last = nt->IsSigned() ? builder->CreateICmpSLE(lhsresult, rhsresult, "OP_LE") : builder->CreateICmpULE(lhsresult, rhsresult, "OP_LE");
            else
                last = builder->CreateFCmpULE(lhsresult, rhsresult, "OP_LE");
            return;
        }
    }
    void CodeGen::Visit(Ides::AST::BinaryExpression<OP_GT>* ast) { SETTRACE("CodeGen::Visit(BinaryExpression<OP_GT>)")
        const Ides::Types::Type* resultType = ast->GetType(actx);
        
        if (resultType->IsNumericType()) {
            auto nt = static_cast<const Ides::Types::NumberType*>(resultType);
            llvm::Value* lhsresult = GetValue(ast->lhs.get());
            llvm::Value* rhsresult = GetValue(ast->rhs.get(), ast->lhs->GetType(actx));
            
            if (nt->IsIntegerType())
                last = nt->IsSigned() ? builder->CreateICmpSGT(lhsresult, rhsresult, "OP_GT") : builder->CreateICmpUGT(lhsresult, rhsresult, "OP_GT");
            else
                last = builder->CreateFCmpUGT(lhsresult, rhsresult, "OP_GT");
            return;
        }
    }
    void CodeGen::Visit(Ides::AST::BinaryExpression<OP_GE>* ast) { SETTRACE("CodeGen::Visit(BinaryExpression<OP_GE>)")
        const Ides::Types::Type* resultType = ast->GetType(actx);
        
        if (resultType->IsNumericType()) {
            auto nt = static_cast<const Ides::Types::NumberType*>(resultType);
            llvm::Value* lhsresult = GetValue(ast->lhs.get());
            llvm::Value* rhsresult = GetValue(ast->rhs.get(), ast->lhs->GetType(actx));
            
            if (nt->IsIntegerType())
                last = nt->IsSigned() ? builder->CreateICmpSGE(lhsresult, rhsresult, "OP_GE") : builder->CreateICmpUGE(lhsresult, rhsresult, "OP_GE");
            else
                last = builder->CreateFCmpUGE(lhsresult, rhsresult, "OP_GE");
            return;
        }
    }
    
    
    
    void CodeGen::Visit(Ides::AST::ConstantStringExpression* ast) { SETTRACE("CodeGen::Visit(ConstantStringExpression)")
        this->Diag(Ides::Diagnostics::COMPILER_NOT_IMPLEMENTED, ast);
    }
    
    void CodeGen::Visit(Ides::AST::ConstantCStringExpression* ast) { SETTRACE("CodeGen::Visit(ConstantCStringExpression)")
        last = builder->CreateGlobalStringPtr(ast->GetBuffer().str());
    }
    
    void CodeGen::Visit(Ides::AST::ConstantWCStringExpression* ast) { SETTRACE("CodeGen::Visit(ConstantWCStringExpression)")
        this->Diag(Ides::Diagnostics::COMPILER_NOT_IMPLEMENTED, ast);
    }
    
    void CodeGen::Visit(Ides::AST::ConstantLCStringExpression* ast) { SETTRACE("CodeGen::Visit(ConstantLCStringExpression)")
        this->Diag(Ides::Diagnostics::COMPILER_NOT_IMPLEMENTED, ast);
    }
    
    void CodeGen::Visit(Ides::AST::ConstantBoolExpression* ast) { SETTRACE("CodeGen::Visit(ConstantBoolExpression)")
        last = ast->GetValue() ? llvm::ConstantInt::getTrue(lctx) : llvm::ConstantInt::getFalse(lctx);
    }
    
    void CodeGen::Visit(Ides::AST::ConstantIntExpression* ast) { SETTRACE("CodeGen::Visit(ConstantIntExpression)")
        last = llvm::ConstantInt::get(this->GetLLVMType(ast->GetType(actx)), ast->GetValue());
    }
    
    void CodeGen::Visit(Ides::AST::ConstantFloatExpression* ast) { SETTRACE("CodeGen::Visit(ConstantFloatExpression)")
        last = llvm::ConstantFP::get(this->GetLLVMType(ast->GetType(actx)), ast->GetValue());
    }


}
}