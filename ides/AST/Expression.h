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
#include <ides/AST/DeclarationContext.h>

#include <ides/Types/Type.h>

namespace Ides {
namespace AST {
    
    class Expression : public Statement, public DeclarationContext {
    public:
        virtual const Ides::Types::Type* GetType(ASTContext& ctx) const = 0;
        
        virtual Declaration* GetMember(ASTContext& ctx, Ides::StringRef name) const {
            return this->GetType(ctx)->GetMember(ctx, name);
        }
        
        virtual void AddMember(Ides::StringRef name, Declaration* decl) { }

    };
    
    class IdentifierExpression : public Expression {
    public:
        IdentifierExpression(Token* tok) : tok(tok) { }
        
        virtual void Accept(Visitor* v) { v->Visit(this); }
        virtual const Ides::Types::Type* GetType(ASTContext& ctx) const;
        
        Ides::StringRef GetName() const { return **tok; }
    private:
        boost::scoped_ptr<Token> tok;
        
    };
    
    typedef std::list<Expression*> ExpressionList;
    
    class UnitTypeExpression : public Expression {
    public:
        virtual void Accept(Visitor* v) { v->Visit(this); }
        virtual const Ides::Types::Type* GetType(ASTContext& ctx) const {
            return Ides::Types::UnitType::GetSingletonPtr();
        }
    };
    
    class NullExpression : public Expression {
        NullExpression() { }
    public:
        static NullExpression* Create() { return new NullExpression(); }
        virtual void Accept(Visitor* v) { v->Visit(this); }
        
        virtual const Ides::Types::Type* GetType(ASTContext& ctx) const { return NULL; }
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
            return retVal.get() ? retVal->GetType(ctx) : Ides::Types::VoidType::GetSingletonPtr();
        }
    private:
        boost::scoped_ptr<Expression> retVal;
    };
    
    class FunctionCallExpression : public Expression {
        FunctionCallExpression(Expression* fn) : fn(fn) { }
    public:
        virtual void Accept(Visitor* v) { v->Visit(this); }
        virtual const Ides::Types::Type* GetType(ASTContext& ctx) const;
        
        static FunctionCallExpression* Create(Expression* fn, ExpressionList* args){
            FunctionCallExpression* ret = Create(fn);
            std::copy(args->begin(), args->end(), std::back_inserter(ret->args));
            delete args;
            return ret;
        }
        static FunctionCallExpression* Create(Expression* fn) {
            return new FunctionCallExpression(fn);
        }
        
        Expression* GetFunction() const { return fn.get(); }
        const ExpressionList& GetArgs() const { return args; }
    private:
        boost::scoped_ptr<Expression> fn;
        ExpressionList args;
    };
    
    class DotExpression : public Expression {
    public:
        DotExpression(Expression* lhs, Token* rhs) : lhs(lhs), rhs(rhs) { }
        virtual void Accept(Visitor* v) { v->Visit(this); }
        virtual const Ides::Types::Type* GetType(ASTContext& ctx) const;
        
        Expression& GetExpression() const { return *lhs; }
        Token& GetToken() const { return *rhs; }
        
    private:
        boost::scoped_ptr<Expression> lhs;
        boost::scoped_ptr<Token> rhs;
    };
    
    class UnaryExpression : public Expression {
    public:
        enum UnaryExpressionType {
            UNARY_POSTFIX,
            UNARY_PREFIX
        };
        
        UnaryExpression(UnaryExpressionType type, Token* func, Expression* arg) : type(type), func(func), arg(arg) { }
        virtual void Accept(Visitor* v) { v->Visit(this); }
        
        virtual const Ides::Types::Type* GetType(ASTContext& ctx) const { return NULL; }
        
        UnaryExpressionType type;
        boost::scoped_ptr<Token> func;
        boost::scoped_ptr<Expression> arg;
    };
    
    class AddressOfExpression : public Expression {
    public:
        AddressOfExpression(Expression* exp) : arg(exp) { }
        virtual void Accept(Visitor* v) { v->Visit(this); }
        
        virtual const Ides::Types::Type* GetType(ASTContext& ctx) const {
            return Ides::Types::PointerType::Get(arg->GetType(ctx));
        }
        
        boost::scoped_ptr<Expression> arg;
    };
    
    class DereferenceExpression : public Expression {
    public:
        DereferenceExpression(Expression* exp) : arg(exp) { }
        virtual void Accept(Visitor* v) { v->Visit(this); }
        
        virtual const Ides::Types::Type* GetType(ASTContext& ctx) const {
            if (const Ides::Types::PointerType* pt = dynamic_cast<const Ides::Types::PointerType*>(arg->GetType(ctx))) {
                return pt->GetTargetType();
            }
            return NULL;
        }
        
        boost::scoped_ptr<Expression> arg;
    };
    
    class InfixExpression : public Expression {
    public:
        InfixExpression(Token* op, Expression* lhs, Expression* rhs) : func(op), lhs(lhs), rhs(rhs) { }
        virtual void Accept(Visitor* v) { v->Visit(this); }
        
        virtual const Ides::Types::Type* GetType(ASTContext& ctx) const { return NULL; }
        
        boost::scoped_ptr<Token> func;
        boost::scoped_ptr<Expression> lhs;
        boost::scoped_ptr<Expression> rhs;
    };
    
    class AssignmentExpression : public Expression {
    public:
        AssignmentExpression(Expression* lhs, Expression* rhs) : lhs(lhs), rhs(rhs) { }
        virtual void Accept(Visitor* v) { v->Visit(this); }
        
        virtual const Ides::Types::Type* GetType(ASTContext& ctx) const { return rhs->GetType(ctx); }
        
        Expression* GetLHS() { return lhs.get(); }
        Expression* GetRHS() { return rhs.get(); }
    private:
        boost::scoped_ptr<Expression> lhs;
        boost::scoped_ptr<Expression> rhs;
    };
}
}

#endif /* defined(__ides__Expression__) */
