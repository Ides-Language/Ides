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
        
        ASTDeclaration(VarType vartype, ASTIdentifier* name, ASTType* type) :
            vartype(vartype), name(name), type(type), initval(NULL) {}
        
        ASTDeclaration(VarType vartype, ASTIdentifier* name, ASTExpression* initval) :
            vartype(vartype), name(name), type(NULL), initval(initval) {}
        
        ASTDeclaration(VarType vartype, ASTIdentifier* name, ASTType* type, ASTExpression* initval) :
            vartype(vartype), name(name), type(type), initval(initval) {}
        
        virtual ~ASTDeclaration();
        
        virtual Ides::String GetDOT() const;
        virtual Ides::String GetCHeader() const;
        
        virtual llvm::Value* GetValue(llvm::IRBuilder<>* builder, SymbolTable& scope) { return this->val; }
        virtual const Ides::Types::Type* GetType(llvm::IRBuilder<>* builder, SymbolTable& scope) {
            return type->GetType(builder, scope);
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
        {
            if (val) val->symbols = this->symbols;
            if (body) body->symbols = this->symbols;
        }
        virtual ~ASTFunction();
        
        virtual Ides::String GetDOT() const;
        virtual Ides::String GetCHeader() const;
        
        virtual Ides::String GetMangledName() const { return this->name->name; }
        
        virtual llvm::Value* GetValue(llvm::IRBuilder<>* builder, SymbolTable& scope);
        virtual const Ides::Types::Type* GetType(llvm::IRBuilder<>* builder, SymbolTable& scope);
        void GenBody(llvm::IRBuilder<>* builder, SymbolTable& scope);
        
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
        
        virtual Ides::String GetDOT() const;
        
        virtual llvm::Value* GetValue(llvm::IRBuilder<>* builder, SymbolTable& scope);
        virtual const Ides::Types::Type* GetType(llvm::IRBuilder<>* builder, SymbolTable& scope);
        
        ASTExpression* fn;
        ASTList* args;
    };
    
    class ASTBracketCall : public ASTExpression {
    public:
        ASTBracketCall(ASTExpression* fn, ASTList* args) : fn(fn), args(args) {}
        virtual ~ASTBracketCall() { delete fn; if(args) delete args; }
        
        virtual Ides::String GetDOT() const;
        
        ASTExpression* fn;
        ASTList* args;
    };
    
    
    class ASTBinaryExpression : public ASTExpression {
    public:
        ASTBinaryExpression(ASTExpression* lhs, ASTExpression* rhs) : lhs(lhs), rhs(rhs) { }
        virtual ~ASTBinaryExpression();
        
        virtual Ides::String GetDOT() const;
        
        ASTExpression* lhs;
        ASTExpression* rhs;
    };
    
    
    class ASTUnaryExpression : public ASTExpression {
    public:
        ASTUnaryExpression(ASTExpression* arg) : arg(arg) { }
        virtual ~ASTUnaryExpression() { delete arg; }
        
        ASTExpression* arg;
    };
    
    class ASTInfixExpression : public ASTBinaryExpression {
    public:
        ASTInfixExpression(ASTIdentifier* func, ASTExpression* lhs, ASTExpression* rhs) : ASTBinaryExpression(lhs, rhs), func(func) { }
        virtual ~ASTInfixExpression() { delete func; }
        
        virtual Ides::String GetDOT() const;
        
        ASTIdentifier* func;
    };
    
    class ASTDictExpression : public ASTList {
        virtual ~ASTDictExpression() { }
        virtual Ides::String GetDOT() const;
    };
    
#define BINARY_EXPRESSION(n) class AST##n##Expression : public ASTBinaryExpression { \
    public: \
        AST##n##Expression(ASTExpression* lhs, ASTExpression* rhs) : ASTBinaryExpression(lhs, rhs) { } \
        virtual Ides::String GetDOT() const; \
        virtual llvm::Value* GetValue(llvm::IRBuilder<>* builder, SymbolTable& scope); \
        virtual const Ides::Types::Type* GetType(llvm::IRBuilder<>* builder, SymbolTable& scope); \
}
    
#define UNARY_EXPRESSION(n) class AST##n##Expression : public ASTUnaryExpression { \
    public: \
        AST##n##Expression(ASTExpression* arg) : ASTUnaryExpression(arg) { } \
        virtual Ides::String GetDOT() const; \
        virtual llvm::Value* GetValue(llvm::IRBuilder<>* builder, SymbolTable& scope); \
        virtual const Ides::Types::Type* GetType(llvm::IRBuilder<>* builder, SymbolTable& scope); \
    }
    
    UNARY_EXPRESSION(Deref);
    UNARY_EXPRESSION(Not);
    UNARY_EXPRESSION(Bnot);
    UNARY_EXPRESSION(Neg);
    UNARY_EXPRESSION(PreInc);
    UNARY_EXPRESSION(PreDec);
    UNARY_EXPRESSION(PostInc);
    UNARY_EXPRESSION(PostDec);
    UNARY_EXPRESSION(New);
    UNARY_EXPRESSION(Throw);
    UNARY_EXPRESSION(Return);
    
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
