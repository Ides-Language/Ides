//
//  ASTVisitor.h
//  ides
//
//  Created by Sean Edwards on 12/30/12.
//
//

#include <ides/AST/AST.h>
#include <ides/AST/Expression.h>
#include <ides/AST/Declaration.h>
#include <ides/AST/Statement.h>
#include <ides/AST/ConstantExpression.h>

#include <ides/parser.hpp>

#ifndef __ides__ASTVisitor__
#define __ides__ASTVisitor__


namespace Ides {
namespace AST {


    class Visitor {
    public:
        virtual void Visit(Ides::AST::CompilationUnit* ast);
        virtual void Visit(Ides::AST::IfStatement* ast) { }
        virtual void Visit(Ides::AST::WhileStatement* ast) { }
        virtual void Visit(Ides::AST::ForStatement* ast) { }
        virtual void Visit(Ides::AST::Block* ast) { }
        virtual void Visit(Ides::AST::IdentifierExpression* ast) { }
        virtual void Visit(Ides::AST::UnitTypeExpression* ast) { }
        virtual void Visit(Ides::AST::NullExpression* ast) { }
        virtual void Visit(Ides::AST::ReturnExpression* ast) { }
        virtual void Visit(Ides::AST::FunctionCallExpression* ast) { }
        virtual void Visit(Ides::AST::DotExpression* ast) { }
        virtual void Visit(Ides::AST::AddressOfExpression* ast) { }
        virtual void Visit(Ides::AST::DereferenceExpression* ast) { }
        virtual void Visit(Ides::AST::InfixExpression* ast) { }
        virtual void Visit(Ides::AST::AssignmentExpression* ast) { }
        virtual void Visit(Ides::AST::ConstantStringExpression* ast) { }
        virtual void Visit(Ides::AST::ConstantCStringExpression* ast) { }
        virtual void Visit(Ides::AST::ConstantWCStringExpression* ast) { }
        virtual void Visit(Ides::AST::ConstantLCStringExpression* ast) { }
        virtual void Visit(Ides::AST::ConstantBoolExpression* ast) { }
        virtual void Visit(Ides::AST::ConstantIntExpression* ast) { }
        virtual void Visit(Ides::AST::ConstantFloatExpression* ast) { }
        
        virtual void Visit(Ides::AST::Attribute* ast) { }
        virtual void Visit(Ides::AST::ValueDeclaration* ast) { }
        virtual void Visit(Ides::AST::VariableDeclaration* ast) { }
        virtual void Visit(Ides::AST::GlobalVariableDeclaration* ast) { }
        virtual void Visit(Ides::AST::ArgumentDeclaration* ast) { }
        virtual void Visit(Ides::AST::StructDeclaration* ast) { }
        virtual void Visit(Ides::AST::FieldDeclaration* ast) { }
        virtual void Visit(Ides::AST::FunctionDeclaration* ast) { }
        virtual void Visit(Ides::AST::OverloadedFunction* ast) { }
        virtual void Visit(Ides::AST::Token* ast) { }
        virtual void Visit(Ides::AST::Type* ast) { }
        
        virtual void Visit(Ides::AST::CastExpression* ast) { }
        
        
        virtual void Visit(Ides::AST::UnaryExpression<OP_NOT>* ast) { }
        virtual void Visit(Ides::AST::UnaryExpression<OP_BNOT>* ast) { }
        virtual void Visit(Ides::AST::UnaryExpression<OP_MINUS>* ast) { }
        virtual void Visit(Ides::AST::UnaryExpression<OP_INC>* ast) { }
        virtual void Visit(Ides::AST::UnaryExpression<OP_DEC>* ast) { }
        
        virtual void Visit(Ides::AST::BinaryExpression<OP_PLUS>* ast) { }
        virtual void Visit(Ides::AST::BinaryExpression<OP_MINUS>* ast) { }
        virtual void Visit(Ides::AST::BinaryExpression<OP_STAR>* ast) { }
        virtual void Visit(Ides::AST::BinaryExpression<OP_SLASH>* ast) { }
        virtual void Visit(Ides::AST::BinaryExpression<OP_MOD>* ast) { }
        
        virtual void Visit(Ides::AST::BinaryExpression<OP_BAND>* ast) { }
        virtual void Visit(Ides::AST::BinaryExpression<OP_BOR>* ast) { }
        virtual void Visit(Ides::AST::BinaryExpression<OP_BXOR>* ast) { }
        
        virtual void Visit(Ides::AST::BinaryExpression<OP_LSHL>* ast) { }
        virtual void Visit(Ides::AST::BinaryExpression<OP_LSHR>* ast) { }
        virtual void Visit(Ides::AST::BinaryExpression<OP_ASHL>* ast) { }
        virtual void Visit(Ides::AST::BinaryExpression<OP_ASHR>* ast) { }
        
        virtual void Visit(Ides::AST::BinaryExpression<OP_AND>* ast) { }
        virtual void Visit(Ides::AST::BinaryExpression<OP_OR>* ast) { }
        
        virtual void Visit(Ides::AST::BinaryExpression<OP_EQ>* ast) { }
        virtual void Visit(Ides::AST::BinaryExpression<OP_NE>* ast) { }
        virtual void Visit(Ides::AST::BinaryExpression<OP_LT>* ast) { }
        virtual void Visit(Ides::AST::BinaryExpression<OP_LE>* ast) { }
        virtual void Visit(Ides::AST::BinaryExpression<OP_GT>* ast) { }
        virtual void Visit(Ides::AST::BinaryExpression<OP_GE>* ast) { }
    };
    
}
}

#endif /* defined(__ides__ASTVisitor__) */
