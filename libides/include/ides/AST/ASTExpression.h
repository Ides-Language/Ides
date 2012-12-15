//
//  ASTExpression.h
//  ides
//
//  Created by Sean Edwards on 12/14/12.
//
//

#ifndef ides_ASTExpression_h
#define ides_ASTExpression_h

#include <ides/AST/AST.h>
#include <ides/AST/ASTSTatement.h>

namespace Ides {
namespace AST {
    class ASTStatement;
    
    
    class ASTExpression : public ASTStatement {
        
    };
    
    class ASTNullExpr : public ASTExpression {
    public:
        ASTNullExpr() { }
        virtual ~ASTNullExpr() { }
        virtual Ides::String GetDOT() const;
        
    };
    
    
    class ASTDeclaration : public ASTExpression {
    public:
        enum VarType {
            DECL_VAR,
            DECL_VAL
        };
        
        ASTDeclaration(VarType vartype, const ASTIdentifier* name, const ASTType* type) :
            vartype(vartype), name(name), type(type), initval(NULL) {}
        
        ASTDeclaration(VarType vartype, const ASTIdentifier* name, const ASTExpression* initval) :
            vartype(vartype), name(name), type(NULL), initval(initval) {}
        
        ASTDeclaration(VarType vartype, const ASTIdentifier* name, const ASTType* type, const ASTExpression* initval) :
            vartype(vartype), name(name), type(type), initval(initval) {}
        
        virtual ~ASTDeclaration();
        
        virtual Ides::String GetDOT() const;
        
        VarType vartype;
        const ASTIdentifier* name;
        const ASTType* type;
        const ASTExpression* initval;
    };
    
    
    class ASTFunctionDecl : public ASTExpression {
    public:
        ASTFunctionDecl(const ASTIdentifier* name, const ASTList* args, const ASTType* rettype) : name(name), rettype(rettype), args(args) { }
        virtual ~ASTFunctionDecl();
        
        virtual Ides::String GetDOT() const;
        
        const ASTIdentifier* name;
        const ASTType* rettype;
        const ASTList* args;
    };
    
    class ASTFunction : public ASTFunctionDecl {
    public:
        ASTFunction(const ASTIdentifier* name, const ASTList* args, const ASTType* rettype) : ASTFunctionDecl(name, args, rettype), val(NULL), body(NULL) {}
        virtual ~ASTFunction() { if (val) delete val; if(body) delete body; }
        
        virtual Ides::String GetDOT() const;
        
        ASTExpression* val;
        ASTCompoundStatement* body;
    };
    
    class ASTFunctionCall : public ASTExpression {
    public:
        ASTFunctionCall(const ASTExpression* fn, const ASTList* args) : fn(fn), args(args) {}
        virtual ~ASTFunctionCall() { delete fn; if(args) delete args; }
        
        virtual Ides::String GetDOT() const;
        
        const ASTExpression* fn;
        const ASTList* args;
    };
    
    class ASTBracketCall : public ASTExpression {
    public:
        ASTBracketCall(const ASTExpression* fn, const ASTList* args) : fn(fn), args(args) {}
        virtual ~ASTBracketCall() { delete fn; if(args) delete args; }
        
        virtual Ides::String GetDOT() const;
        
        const ASTExpression* fn;
        const ASTList* args;
    };
    
    
    class ASTBinaryExpression : public ASTExpression {
    public:
        ASTBinaryExpression(const ASTExpression* lhs, const ASTExpression* rhs) : lhs(lhs), rhs(rhs) { }
        virtual ~ASTBinaryExpression();
        
        virtual Ides::String GetDOT() const;
        
        const ASTExpression* lhs;
        const ASTExpression* rhs;
    };
    
    
    class ASTUnaryExpression : public ASTExpression {
    public:
        ASTUnaryExpression(const ASTExpression* arg) : arg(arg) { }
        virtual ~ASTUnaryExpression() { delete arg; }
        
        const ASTExpression* arg;
    };
    
    class ASTInfixExpression : public ASTBinaryExpression {
    public:
        ASTInfixExpression(const ASTIdentifier* func, const ASTExpression* lhs, const ASTExpression* rhs) : ASTBinaryExpression(lhs, rhs), func(func) { }
        virtual ~ASTInfixExpression() { delete func; }
        
        virtual Ides::String GetDOT() const;
        
        const ASTIdentifier* func;
    };
    
    class ASTDictExpression : public ASTList {
        virtual ~ASTDictExpression() { }
        virtual Ides::String GetDOT() const;
    };
    
#define BINARY_EXPRESSION(n) class AST##n##Expression : public ASTBinaryExpression { \
    public: \
        AST##n##Expression(const ASTExpression* lhs, const ASTExpression* rhs) : ASTBinaryExpression(lhs, rhs) { } \
        virtual Ides::String GetDOT() const; \
}
    
#define UNARY_EXPRESSION(n) class AST##n##Expression : public ASTUnaryExpression { \
    public: \
        AST##n##Expression(const ASTExpression* arg) : ASTUnaryExpression(arg) { } \
        virtual Ides::String GetDOT() const; \
    }
    
    UNARY_EXPRESSION(Deref);
    UNARY_EXPRESSION(Not);
    UNARY_EXPRESSION(Bnot);
    UNARY_EXPRESSION(Neg);
    UNARY_EXPRESSION(PreInc);
    UNARY_EXPRESSION(PreDec);
    UNARY_EXPRESSION(PostInc);
    UNARY_EXPRESSION(PostDec);
    
    BINARY_EXPRESSION(Eq);
    BINARY_EXPRESSION(Add);
    BINARY_EXPRESSION(Sub);
    BINARY_EXPRESSION(Mul);
    BINARY_EXPRESSION(Div);
    BINARY_EXPRESSION(Mod);
    BINARY_EXPRESSION(Band);
    BINARY_EXPRESSION(Bor);
    BINARY_EXPRESSION(Bxor);
    
    BINARY_EXPRESSION(Coalesce);
    BINARY_EXPRESSION(Cast);
    
    BINARY_EXPRESSION(DictPair);
    
    
} // namespace AST
} // namespace Ides

#endif
