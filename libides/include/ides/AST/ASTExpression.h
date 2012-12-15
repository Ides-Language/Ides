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

namespace Ides {
namespace AST {
    
    class ASTExpression : public AST {
        
    };
    
    
    
    class ASTDeclaration : public ASTExpression {
    public:
        
        enum VarType {
            DECL_VAR,
            DECL_VAL
        };
        
        ASTDeclaration(VarType vartype, const ASTIdentifier* name, const ASTIdentifier* type) :
            vartype(vartype), name(name), type(type), initval(NULL) {}
        
        ASTDeclaration(VarType vartype, const ASTIdentifier* name, const ASTExpression* initval) :
            vartype(vartype), name(name), type(NULL), initval(initval) {}
        
        ASTDeclaration(VarType vartype, const ASTIdentifier* name, const ASTIdentifier* type, const ASTExpression* initval) :
            vartype(vartype), name(name), type(type), initval(initval) {}
        
        virtual Ides::String GetDOT() const;
        
        VarType vartype;
        const ASTIdentifier* name;
        const ASTIdentifier* type;
        const ASTExpression* initval;
    };
    
    
    
    
    class ASTFunction : public ASTExpression {
    public:
        ASTFunction(const ASTIdentifier* name, const ASTList* args, const ASTIdentifier* rettype) : name(name), rettype(rettype), args(args) {}
        virtual Ides::String GetDOT() const;
        
        const ASTIdentifier* name;
        const ASTIdentifier* rettype;
        const ASTList* args;
        ASTExpression* val;
    };
    
    
    class ASTBinaryExpression : public ASTExpression {
    public:
        ASTBinaryExpression(const ASTExpression* lhs, const ASTExpression* rhs) : lhs(lhs), rhs(rhs) { }
        
        const ASTExpression* lhs;
        const ASTExpression* rhs;
    };
    
    
    class ASTUnaryExpression : public ASTExpression {
    public:
        ASTUnaryExpression(const ASTExpression* arg) : arg(arg) { }
        
        const ASTExpression* arg;
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
    
    BINARY_EXPRESSION(Eq);
    BINARY_EXPRESSION(Add);
    BINARY_EXPRESSION(Sub);
    BINARY_EXPRESSION(Mul);
    BINARY_EXPRESSION(Div);
    BINARY_EXPRESSION(Mod);
    BINARY_EXPRESSION(Coalesce);
    BINARY_EXPRESSION(Band);
    BINARY_EXPRESSION(Bor);
    BINARY_EXPRESSION(Bxor);
    
    
} // namespace AST
} // namespace Ides

#endif
