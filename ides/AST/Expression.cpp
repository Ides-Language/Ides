//
//  Expression.cpp
//  ides
//
//  Created by Sean Edwards on 12/30/12.
//
//

#include "Expression.h"
#include "Declaration.h"
#include <ides/ASTVisitor/ASTVisitor.h>

namespace Ides {
namespace Util {
    
}
    
namespace AST {
    
    const Ides::Types::Type* DotExpression::GetType(ASTContext& ctx) const {
        const Ides::Types::Type* lhstype = lhs->GetType(ctx);
        Declaration* decl = lhstype->GetMember(ctx, **rhs);
        if (decl) return decl->GetType(ctx);
        throw Ides::AST::TypeEvalError(ctx.GetDiagnostics(), Ides::Diagnostics::UNKNOWN_MEMBER, rhs->exprloc) << lhstype->ToString() << **rhs;
    }
    
    const Ides::Types::Type* IdentifierExpression::GetType(ASTContext& ctx) const {
        Declaration* decl = ctx.GetCurrentScope()->GetMember(ctx, **tok);
        if (decl) return decl->GetType(ctx);
        throw Ides::AST::TypeEvalError(ctx.GetDiagnostics(), Ides::Diagnostics::UNKNOWN_IDENTIFIER, this->exprloc) << **tok;
    }
    
    const Ides::Types::Type* FunctionCallExpression::GetType(ASTContext& ctx) const {
        const Ides::Types::Type* ft = fn->GetType(ctx);
        const Ides::Types::FunctionType* fntype = dynamic_cast<const Ides::Types::FunctionType*>(ft);
        if (fn) return fntype->retType;
        throw Ides::AST::TypeEvalError(ctx.GetDiagnostics(), Ides::Diagnostics::CALL_NON_FUNCTION, this->exprloc) << ft->ToString();
    }
    
    
    
    void IdentifierExpression::Accept(Visitor* v) { v->Visit(this); }
    void UnitTypeExpression::Accept(Visitor* v) { v->Visit(this); }
    void NullExpression::Accept(Visitor* v) { v->Visit(this); }
    void ReturnExpression::Accept(Visitor* v) { v->Visit(this); }
    void FunctionCallExpression::Accept(Visitor* v) { v->Visit(this); }
    void DotExpression::Accept(Visitor* v) { v->Visit(this); }
    void UnaryExpression::Accept(Visitor* v) { v->Visit(this); }
    void AddressOfExpression::Accept(Visitor* v) { v->Visit(this); }
    void DereferenceExpression::Accept(Visitor* v) { v->Visit(this); }
    void InfixExpression::Accept(Visitor* v) { v->Visit(this); }
    void AssignmentExpression::Accept(Visitor* v) { v->Visit(this); }
    
    void CastExpression::Accept(Visitor* v) { v->Visit(this); }
    
    template<> const Ides::Types::Type* BinaryExpression<OP_ASHL>::GetType(Ides::AST::ASTContext &ctx) const { return lhs->GetType(ctx); }
    template<> const Ides::Types::Type* BinaryExpression<OP_ASHR>::GetType(Ides::AST::ASTContext &ctx) const { return lhs->GetType(ctx); }
    template<> const Ides::Types::Type* BinaryExpression<OP_LSHL>::GetType(Ides::AST::ASTContext &ctx) const { return lhs->GetType(ctx); }
    template<> const Ides::Types::Type* BinaryExpression<OP_LSHR>::GetType(Ides::AST::ASTContext &ctx) const { return lhs->GetType(ctx); }
    
    template<> const Ides::Types::Type* BinaryExpression<OP_AND>::GetType(Ides::AST::ASTContext &ctx) const { return Ides::Types::Integer1Type::GetSingletonPtr(); }
    template<> const Ides::Types::Type* BinaryExpression<OP_OR>::GetType(Ides::AST::ASTContext &ctx) const { return Ides::Types::Integer1Type::GetSingletonPtr(); }
    
    template<> const Ides::Types::Type* BinaryExpression<OP_EQ>::GetType(Ides::AST::ASTContext &ctx) const { return Ides::Types::Integer1Type::GetSingletonPtr(); }
    template<> const Ides::Types::Type* BinaryExpression<OP_NE>::GetType(Ides::AST::ASTContext &ctx) const { return Ides::Types::Integer1Type::GetSingletonPtr(); }
    template<> const Ides::Types::Type* BinaryExpression<OP_LT>::GetType(Ides::AST::ASTContext &ctx) const { return Ides::Types::Integer1Type::GetSingletonPtr(); }
    template<> const Ides::Types::Type* BinaryExpression<OP_LE>::GetType(Ides::AST::ASTContext &ctx) const { return Ides::Types::Integer1Type::GetSingletonPtr(); }
    template<> const Ides::Types::Type* BinaryExpression<OP_GT>::GetType(Ides::AST::ASTContext &ctx) const { return Ides::Types::Integer1Type::GetSingletonPtr(); }
    template<> const Ides::Types::Type* BinaryExpression<OP_GE>::GetType(Ides::AST::ASTContext &ctx) const { return Ides::Types::Integer1Type::GetSingletonPtr(); }
    
    template<> void BinaryExpression<0>::Accept(Visitor* v) { }
    
    template<> void BinaryExpression<OP_PLUS>::Accept(Visitor* v) { v->Visit(this); }
    template<> void BinaryExpression<OP_MINUS>::Accept(Visitor* v) { v->Visit(this); }
    template<> void BinaryExpression<OP_STAR>::Accept(Visitor* v) { v->Visit(this); }
    template<> void BinaryExpression<OP_SLASH>::Accept(Visitor* v) { v->Visit(this); }
    template<> void BinaryExpression<OP_MOD>::Accept(Visitor* v) { v->Visit(this); }
    
    
    template<> void BinaryExpression<OP_BAND>::Accept(Visitor* v) { v->Visit(this); }
    template<> void BinaryExpression<OP_BOR>::Accept(Visitor* v) { v->Visit(this); }
    template<> void BinaryExpression<OP_BXOR>::Accept(Visitor* v) { v->Visit(this); }
    
    template<> void BinaryExpression<OP_LSHL>::Accept(Visitor* v) { v->Visit(this); }
    template<> void BinaryExpression<OP_LSHR>::Accept(Visitor* v) { v->Visit(this); }
    template<> void BinaryExpression<OP_ASHL>::Accept(Visitor* v) { v->Visit(this); }
    template<> void BinaryExpression<OP_ASHR>::Accept(Visitor* v) { v->Visit(this); }
    
    template<> void BinaryExpression<OP_AND>::Accept(Visitor* v) { v->Visit(this); }
    template<> void BinaryExpression<OP_OR>::Accept(Visitor* v) { v->Visit(this); }
    
    template<> void BinaryExpression<OP_EQ>::Accept(Visitor* v) { v->Visit(this); }
    template<> void BinaryExpression<OP_NE>::Accept(Visitor* v) { v->Visit(this); }
    template<> void BinaryExpression<OP_LT>::Accept(Visitor* v) { v->Visit(this); }
    template<> void BinaryExpression<OP_LE>::Accept(Visitor* v) { v->Visit(this); }
    template<> void BinaryExpression<OP_GT>::Accept(Visitor* v) { v->Visit(this); }
    template<> void BinaryExpression<OP_GE>::Accept(Visitor* v) { v->Visit(this); }
}
}