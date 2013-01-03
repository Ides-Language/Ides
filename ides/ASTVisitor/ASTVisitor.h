//
//  ASTVisitor.h
//  ides
//
//  Created by Sean Edwards on 12/30/12.
//
//

#include <ides/Types/Type.h>

#ifndef __ides__ASTVisitor__
#define __ides__ASTVisitor__


namespace Ides {
namespace AST {
    
    class CompilationUnit;
    
    class IfStatement;
    class WhileStatement;
    class ForStatement;
    
    class Block;
    
    class IdentifierExpression;
    class UnitTypeExpression;
    class NullExpression;
    class ReturnExpression;
    class FunctionCallExpression;
    class DotExpression;
    
    class UnaryExpression;
    class InfixExpression;
    class AssignmentExpression;
    
    class ConstantStringExpression;
    class ConstantCStringExpression;
    class ConstantWCStringExpression;
    class ConstantLCStringExpression;
    
    template<typename Tt, typename Vt>
    class ConstantBuiltinTypeExpression;
    
    typedef ConstantBuiltinTypeExpression<Ides::Types::Integer1Type, bool> ConstantBoolExpression;
    typedef ConstantBuiltinTypeExpression<Ides::Types::Integer8Type, uint8_t> ConstantCharExpression;
    typedef ConstantBuiltinTypeExpression<Ides::Types::Integer32Type, uint64_t> ConstantIntExpression;
    typedef ConstantBuiltinTypeExpression<Ides::Types::Float64Type, double> ConstantFloatExpression;
    
    class ValueDeclaration;
    class VariableDeclaration;
    class StructDeclaration;
    
    class FunctionDeclaration;
    class OverloadedFunction;
    
    class Token;
    class Type;
    
    
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
        virtual void Visit(Ides::AST::UnaryExpression* ast) { }
        virtual void Visit(Ides::AST::InfixExpression* ast) { }
        virtual void Visit(Ides::AST::AssignmentExpression* ast) { }
        virtual void Visit(Ides::AST::ConstantStringExpression* ast) { }
        virtual void Visit(Ides::AST::ConstantCStringExpression* ast) { }
        virtual void Visit(Ides::AST::ConstantWCStringExpression* ast) { }
        virtual void Visit(Ides::AST::ConstantLCStringExpression* ast) { }
        virtual void Visit(Ides::AST::ConstantBoolExpression* ast) { }
        virtual void Visit(Ides::AST::ConstantCharExpression* ast) { }
        virtual void Visit(Ides::AST::ConstantIntExpression* ast) { }
        virtual void Visit(Ides::AST::ConstantFloatExpression* ast) { }
        virtual void Visit(Ides::AST::ValueDeclaration* ast) { }
        virtual void Visit(Ides::AST::VariableDeclaration* ast) { }
        virtual void Visit(Ides::AST::StructDeclaration* ast) { }
        virtual void Visit(Ides::AST::FunctionDeclaration* ast) { }
        virtual void Visit(Ides::AST::OverloadedFunction* ast) { }
        virtual void Visit(Ides::AST::Token* ast) { }
        virtual void Visit(Ides::AST::Type* ast) { }
    };
    
}
}

#endif /* defined(__ides__ASTVisitor__) */
