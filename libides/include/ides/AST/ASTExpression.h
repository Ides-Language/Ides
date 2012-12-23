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
        
        virtual llvm::Value* GetValue(ParseContext& ctx);
        virtual const Ides::Types::Type* GetType(ParseContext& ctx);
        
        virtual llvm::Value* GetConvertedValue(ParseContext& ctx, const Ides::Types::Type* to);
        virtual llvm::Value* GetConvertedValue(ParseContext& ctx);
        
        llvm::Value* val;
        
        VarType vartype;
        ASTIdentifier* name;
        ASTType* type;
        ASTExpression* initval;
    };
    
    class ASTFunction : public ASTExpression {
    public:
        ASTFunction(ASTIdentifier* name, ASTList* args, ASTType* rettype) :
            func(NULL), retblock(NULL), returnvalue(NULL), functype(NULL),
            name(name), args(args), val(NULL), body(NULL), returntype(rettype), evaluatingtype(false)
        { }
        virtual ~ASTFunction();
        
        virtual Ides::String GetMangledName() const { return this->name->name; }
        
        virtual llvm::Value* GetValue(ParseContext& ctx);
        virtual const Ides::Types::Type* GetType(ParseContext& ctx);
        
        const Ides::Types::Type* GetReturnType(ParseContext& ctx);
        
        void GenBody(ParseContext& ctx);
        
        llvm::Function* func;
        llvm::BasicBlock* retblock;
        llvm::Value* returnvalue;
        
        const Ides::Types::FunctionType* functype;
        
        ASTIdentifier* name;
        ASTList* args;
        
        ASTExpression* val;
        ASTCompoundStatement* body;
        
    private:
        ASTType* returntype;
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
    
    class ASTReturnExpression : public ASTExpression {
    public:
        ASTReturnExpression(ASTExpression* retval) : retval(retval) {}
        virtual ~ASTReturnExpression() { delete retval; }
        
        virtual llvm::Value* GetValue(ParseContext& ctx);
        virtual const Ides::Types::Type* GetType(ParseContext& ctx);
    
        ASTExpression* retval;
    };
    
    class ASTCastExpression : public ASTExpression {
    public:
        ASTCastExpression(ASTExpression* lhs, ASTType* rhs) : lhs(lhs), rhs(rhs) { }
        virtual ~ASTCastExpression() {
            delete lhs;
            delete rhs;
        }
        
        virtual llvm::Value* GetValue(ParseContext& ctx);
        virtual const Ides::Types::Type* GetType(ParseContext& ctx);
        
        ASTExpression* lhs;
        ASTType* rhs;
    };
    
    class ASTAddressOfExpression : public ASTExpression {
    public:
        ASTAddressOfExpression(ASTExpression* arg) : arg(arg) { }
        ~ASTAddressOfExpression() { }
        
        virtual llvm::Value* GetValue(ParseContext& ctx);
        virtual const Ides::Types::Type* GetType(ParseContext& ctx);
        
        ASTExpression* arg;
    };
    
    class ASTDereferenceExpression : public ASTExpression {
    public:
        ASTDereferenceExpression(ASTExpression* arg) : arg(arg) { }
        ~ASTDereferenceExpression() { }
        
        virtual llvm::Value* GetValue(ParseContext& ctx);
        virtual const Ides::Types::Type* GetType(ParseContext& ctx);
        
        ASTExpression* arg;
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
            if (func) delete func;
            delete lhs;
            delete rhs;
        }
        
        virtual llvm::Value* GetValue(ParseContext& ctx);
        virtual const Ides::Types::Type* GetType(ParseContext& ctx);
        
        ASTIdentifier* func;
        ASTExpression* lhs;
        ASTExpression* rhs;
    };
    
    class ASTAssignmentExpression : public ASTInfixExpression {
    public:
        ASTAssignmentExpression(ASTExpression* lhs, ASTExpression* rhs) : ASTInfixExpression(NULL, lhs, rhs) { }
        virtual ~ASTAssignmentExpression() { }
        
        virtual llvm::Value* GetValue(ParseContext& ctx);
        virtual const Ides::Types::Type* GetType(ParseContext& ctx);
    };
    
    class ASTDictExpression : public ASTList {
        virtual ~ASTDictExpression() { }
    };
    
    
} // namespace AST
} // namespace Ides

#endif
