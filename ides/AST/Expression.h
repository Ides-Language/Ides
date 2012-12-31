//
//  Expression.h
//  ides
//
//  Created by Sean Edwards on 12/30/12.
//
//

#ifndef __ides__Expression__
#define __ides__Expression__

#include <ides/AST/AST.h>
#include <ides/AST/Statement.h>

#include <ides/Types/Type.h>

namespace Ides {
namespace AST {
    
    class Expression : public Statement {
    public:
        virtual const Ides::Types::Type* GetType(ASTContext& ctx) = 0;

    };
    
    typedef std::list<boost::shared_ptr<Expression> > ExpressionList;
    
    class UnitTypeExpression : public Expression {
    public:
        virtual void Accept(Visitor* v) { v->Visit(this); }
        virtual const Ides::Types::Type* GetType(ASTContext& ctx) {
            return Ides::Types::UnitType::GetSingletonPtr();
        }
    };
    
    class NullExpression : public Expression {
        NullExpression() { }
    public:
        static NullExpression* Create() { return new NullExpression(); }
        virtual void Accept(Visitor* v) { v->Visit(this); }
        
        virtual const Ides::Types::Type* GetType(ASTContext& ctx) { return NULL; }
    };
    
    class ReturnExpression : public UnitTypeExpression {
        ReturnExpression() { }
        ReturnExpression(Expression* ret) : retVal(ret) { }
    public:
        virtual void Accept(Visitor* v) { v->Visit(this); }
        
        static ReturnExpression* Create() { return new ReturnExpression(); }
        static ReturnExpression* Create(Expression* ret) { return new ReturnExpression(ret); }
        
        Expression* GetRetVal() const { return retVal.get(); }
        const Ides::Types::Type* GetRetType(ASTContext& ctx) const {
            return retVal ? retVal->GetType(ctx) : Ides::Types::VoidType::GetSingletonPtr();
        }
    private:
        boost::scoped_ptr<Expression> retVal;
    };
    
    class FunctionCallExpression : public Expression {
        FunctionCallExpression(Expression* fn) : fn(fn) { }
    public:
        virtual void Accept(Visitor* v) { v->Visit(this); }
        virtual const Ides::Types::Type* GetType(ASTContext& ctx) { return NULL; }
        
        static FunctionCallExpression* Create(Expression* fn, ExpressionList* args){
            FunctionCallExpression* ret = Create(fn);
            std::copy(args->begin(), args->end(), ret->args.begin());
            return ret;
        }
        static FunctionCallExpression* Create(Expression* fn) {
            return new FunctionCallExpression(fn);
        }
        
        const Expression& GetFunction() const { return *fn; }
        const ExpressionList& GetArgs() const { return args; }
    private:
        boost::scoped_ptr<Expression> fn;
        ExpressionList args;
    };
    
    class UnaryExpression : public Expression {
    public:
        enum UnaryExpressionType {
            UNARY_POSTFIX,
            UNARY_PREFIX
        };
        
        UnaryExpression(UnaryExpressionType type, Token* func, Expression* arg) : type(type), func(func), arg(arg) { }
        virtual void Accept(Visitor* v) { v->Visit(this); }
        
        virtual const Ides::Types::Type* GetType(ASTContext& ctx) { return NULL; }
        
        UnaryExpressionType type;
        boost::scoped_ptr<Token> func;
        boost::scoped_ptr<Expression> arg;
    };
    
    class InfixExpression : public Expression {
    public:
        InfixExpression(Token* op, Expression* lhs, Expression* rhs) : func(op), lhs(lhs), rhs(rhs) { }
        virtual void Accept(Visitor* v) { v->Visit(this); }
        
        virtual const Ides::Types::Type* GetType(ASTContext& ctx) { return NULL; }
        
        boost::scoped_ptr<Token> func;
        boost::scoped_ptr<Expression> lhs;
        boost::scoped_ptr<Expression> rhs;
    };
    
    class AssignmentExpression : public Expression {
    public:
        AssignmentExpression(Expression* lhs, Expression* rhs) : lhs(lhs), rhs(rhs) { }
        virtual void Accept(Visitor* v) { v->Visit(this); }
        
        virtual const Ides::Types::Type* GetType(ASTContext& ctx) { return NULL; }
        
        boost::scoped_ptr<Expression> lhs;
        boost::scoped_ptr<Expression> rhs;
    };
}
}

#endif /* defined(__ides__Expression__) */
