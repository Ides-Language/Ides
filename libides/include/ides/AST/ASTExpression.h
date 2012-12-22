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
    };
    
    
    class ASTDeclaration : public ASTExpression {
    public:
        enum VarType {
            DECL_VAR,
            DECL_VAL
        };
        
        ASTDeclaration(VarType vartype, ASTIdentifier* name, ASTType* type) :
            vartype(vartype), name(name), type(type), initval(NULL) {}
        
        ASTDeclaration(VarType vartype, ASTIdentifier* name, ASTExpression* initval) :
            vartype(vartype), name(name), type(NULL), initval(initval) {}
        
        ASTDeclaration(VarType vartype, ASTIdentifier* name, ASTType* type, ASTExpression* initval) :
            vartype(vartype), name(name), type(type), initval(initval) {}
        
        virtual ~ASTDeclaration();
        
        virtual llvm::Value* GetValue(ParseContext& ctx) { return this->val; }
        virtual const Ides::Types::Type* GetType(ParseContext& ctx) {
            return type->GetType(ctx);
        }
        
        llvm::Value* val;
        
        VarType vartype;
        ASTIdentifier* name;
        ASTType* type;
        ASTExpression* initval;
    };
    
    class ASTFunction : public ASTExpression {
    public:
        ASTFunction(ASTIdentifier* name, ASTList* args, ASTType* rettype) :
            func(NULL), name(name), rettype(rettype), args(args), val(NULL), body(NULL), evaluatingtype(false)
        { }
        virtual ~ASTFunction();
        
        virtual Ides::String GetMangledName() const { return this->name->name; }
        
        virtual llvm::Value* GetValue(ParseContext& ctx);
        virtual const Ides::Types::Type* GetType(ParseContext& ctx);
        
        void GenBody(ParseContext& ctx);
        
        llvm::Function* func;
        
        ASTIdentifier* name;
        ASTType* rettype;
        ASTList* args;
        
        ASTExpression* val;
        ASTCompoundStatement* body;
        
        bool evaluatingtype;
    };
    
    class ASTFunctionCall : public ASTExpression {
    public:
        ASTFunctionCall(ASTExpression* fn, ASTList* args) : fn(fn), args(args) {}
        virtual ~ASTFunctionCall() { delete fn; if(args) delete args; }
        
        virtual llvm::Value* GetValue(ParseContext& ctx);
        virtual const Ides::Types::Type* GetType(ParseContext& ctx);
        
        ASTExpression* fn;
        ASTList* args;
    };
    
    class ASTBracketCall : public ASTExpression {
    public:
        ASTBracketCall(ASTExpression* fn, ASTList* args) : fn(fn), args(args) {}
        virtual ~ASTBracketCall() { delete fn; if(args) delete args; }
        
        ASTExpression* fn;
        ASTList* args;
    };
    
    
    class ASTUnaryExpression : public ASTExpression {
    public:
        enum UnaryExpressionType {
            UNARY_POSTFIX,
            UNARY_PREFIX
        };
        
        ASTUnaryExpression(UnaryExpressionType type, ASTIdentifier* func, ASTExpression* arg) : type(type), func(func), arg(arg) { }
        virtual ~ASTUnaryExpression() { delete func; delete arg; }
        
        UnaryExpressionType type;
        ASTIdentifier* func;
        ASTExpression* arg;
    };
    
    class ASTInfixExpression : public ASTExpression {
    public:
        ASTInfixExpression(ASTIdentifier* op, ASTExpression* lhs, ASTExpression* rhs) : func(op), lhs(lhs), rhs(rhs) { }
        virtual ~ASTInfixExpression() {
            delete func;
            delete lhs;
            delete rhs;
        }
        
        virtual llvm::Value* GetValue(ParseContext& ctx);
        virtual const Ides::Types::Type* GetType(ParseContext& ctx);
        
        ASTIdentifier* func;
        ASTExpression* lhs;
        ASTExpression* rhs;
    };
    
    class ASTDictExpression : public ASTList {
        virtual ~ASTDictExpression() { }
    };
    
    
} // namespace AST
} // namespace Ides

#endif
