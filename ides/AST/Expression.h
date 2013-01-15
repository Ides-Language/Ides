 
//
//  Expression.h
//  ides
//
//  Created by Sean Edwards on 12/30/12.
//
//

#ifndef __ides__Expression__
#define __ides__Expression__

#include <ides/AST/Statement.h>
#include <ides/AST/DeclarationContext.h>
#include <ides/AST/AST.h>

#include <ides/Types/Type.h>

namespace Ides {
namespace AST {
    
    class Expression : public Statement {
    public:
        virtual const Ides::Types::Type* GetType(ASTContext& ctx) const = 0;

    };
    
    class IdentifierExpression : public Expression {
    public:
        IdentifierExpression(Token* tok) : tok(tok) { }
        
        virtual void Accept(Visitor* v);
        virtual const Ides::Types::Type* GetType(ASTContext& ctx) const;
        
        Ides::StringRef GetName() const { return **tok; }
    private:
        boost::scoped_ptr<Token> tok;
        
    };
    
    typedef std::vector<Expression*> ExpressionList;
    
    class UnitTypeExpression : public Expression {
    public:
        virtual void Accept(Visitor* v);
        virtual const Ides::Types::Type* GetType(ASTContext& ctx) const {
            return Ides::Types::UnitType::GetSingletonPtr();
        }
    };
    
    class NullExpression : public Expression {
        NullExpression() { }
    public:
        static NullExpression* Create() { return new NullExpression(); }
        virtual void Accept(Visitor* v);
        
        virtual const Ides::Types::Type* GetType(ASTContext& ctx) const {
            return Ides::Types::UnitType::GetSingletonPtr();
        }
    };
    
    class ReturnExpression : public UnitTypeExpression {
        ReturnExpression() { }
        ReturnExpression(Expression* ret) : retVal(ret) { }
    public:
        virtual void Accept(Visitor* v);
        
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
        virtual void Accept(Visitor* v);
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
        virtual void Accept(Visitor* v);
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
        virtual void Accept(Visitor* v);
        
        virtual const Ides::Types::Type* GetType(ASTContext& ctx) const { return NULL; }
        
        UnaryExpressionType type;
        boost::scoped_ptr<Token> func;
        boost::scoped_ptr<Expression> arg;
    };
    
    class AddressOfExpression : public Expression {
    public:
        AddressOfExpression(Expression* exp) : arg(exp) { }
        virtual void Accept(Visitor* v);
        
        virtual const Ides::Types::Type* GetType(ASTContext& ctx) const {
            return Ides::Types::PointerType::Get(arg->GetType(ctx));
        }
        
        boost::scoped_ptr<Expression> arg;
    };
    
    class DereferenceExpression : public Expression {
    public:
        DereferenceExpression(Expression* exp) : arg(exp) { }
        virtual void Accept(Visitor* v);
        
        virtual const Ides::Types::Type* GetType(ASTContext& ctx) const {
            const Ides::Types::Type* argType = arg->GetType(ctx);
            if (const Ides::Types::PointerType* pt = dynamic_cast<const Ides::Types::PointerType*>(argType)) {
                return pt->GetTargetType();
            }
            throw TypeEvalError(ctx.GetDiagnostics(), Ides::Diagnostics::OP_NO_SUCH_OPERATOR, this->exprloc) << "*" << argType->ToString();
        }
        
        boost::scoped_ptr<Expression> arg;
    };
    
    class CastExpression : public Expression {
    public:
        CastExpression(Expression* lhs, Type* rhs) : lhs(lhs), rhs(rhs) { }
        virtual void Accept(Visitor* v);
        
        virtual const Ides::Types::Type* GetType(ASTContext& ctx) const {
            return rhs->GetType(ctx);
        }
        
        boost::scoped_ptr<Expression> lhs;
        boost::scoped_ptr<Type> rhs;
        
    };
    
    template<int op>
    class BinaryExpression : public Expression {
    public:
        BinaryExpression(Expression* lhs, Expression* rhs) : lhs(lhs), rhs(rhs) { }
        virtual void Accept(Visitor* v);
        
        virtual const Ides::Types::Type* GetType(ASTContext& ctx) const {
            const Ides::Types::Type* lhstype = lhs->GetType(ctx);
            const Ides::Types::Type* rhstype = rhs->GetType(ctx);
            
            if (lhstype->IsNumericType()) {
                if (rhstype->IsEquivalentType(lhstype)) return lhstype;
                else if (rhstype->HasImplicitConversionTo(lhstype)) return lhstype;
                else if (lhstype->HasImplicitConversionTo(rhstype)) return rhstype;
                
            }
            
            throw Ides::AST::TypeEvalError(ctx.GetDiagnostics(), Ides::Diagnostics::NO_IMPLICIT_CONVERSION, this->exprloc) << lhstype->ToString() << rhstype->ToString();
        }
        
        boost::scoped_ptr<Expression> lhs;
        boost::scoped_ptr<Expression> rhs;
        
    };
        
    class InfixExpression : public BinaryExpression<0> {
    public:
        InfixExpression(Token* op, Expression* lhs, Expression* rhs) : BinaryExpression(lhs, rhs), func(op) { }
        virtual void Accept(Visitor* v);
        
        virtual const Ides::Types::Type* GetType(ASTContext& ctx) const { return NULL; }
        
        boost::scoped_ptr<Token> func;
    };
    
    class AssignmentExpression : public Expression {
    public:
        AssignmentExpression(Expression* lhs, Expression* rhs) : lhs(lhs), rhs(rhs) { }
        virtual void Accept(Visitor* v);
        
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
