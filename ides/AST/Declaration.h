//
//  Declaration.h
//  ides
//
//  Created by Sean Edwards on 12/30/12.
//
//

#ifndef __ides__Declaration__
#define __ides__Declaration__


#include <ides/AST/Expression.h>
#include <ides/AST/Statement.h>
#include <ides/AST/DeclarationContext.h>
#include <ides/AST/AST.h>

namespace Ides {
namespace AST {
    
    class Declaration : public AST {
    public:
        virtual const Ides::Types::Type* GetType(ASTContext& ctx) = 0;
    };
    
    typedef std::list<Declaration*> DeclarationList;
    
    class NamedDeclaration : public Declaration {
    protected:
        NamedDeclaration(Token* tok) : name(tok) { }
    public:
        Ides::StringRef GetName() const { return **name; }
    private:
        boost::scoped_ptr<Token> name;
    };
    
    class ValueDeclaration : public NamedDeclaration {
    protected:
        ValueDeclaration(Token* tok) : NamedDeclaration(tok) { }
    public:
        virtual void Accept(Visitor* v);
        
        virtual const Ides::Types::Type* GetType(ASTContext& ctx) = 0;
        
    };
    
    class VariableDeclaration : public ValueDeclaration {
    public:
        enum VarType {
            DECL_VAR,
            DECL_VAL
        };
        
        VariableDeclaration(VarType vartype, Token* name, Type* type) : ValueDeclaration(name),
            vartype(vartype), type(type), initval(NULL) {}
        
        VariableDeclaration(VarType vartype, Token* name, Expression* initval) : ValueDeclaration(name),
            vartype(vartype), type(NULL), initval(initval) {}
        
        VariableDeclaration(VarType vartype, Token* name, Type* type, Expression* initval) : ValueDeclaration(name),
            vartype(vartype), type(type), initval(initval) {}
        
        virtual void Accept(Visitor* v);
        
        virtual const Ides::Types::Type* GetType(ASTContext& ctx) {
            if (type) return type->GetType(ctx);
            else if (initval) return initval->GetType(ctx);
            
            assert(false); // This shouldn't be possible according to the language grammar.
            throw Ides::AST::TypeEvalError(ctx.GetDiagnostics(), Ides::Diagnostics::IMPOSSIBLE_ERROR, this->exprloc) <<
                "variable declaration contains neither a type specifier nor an initializer";
        }
        
        VarType vartype;
        Type* type;
        Expression* initval;
    };
    
    class GlobalVariableDeclaration : public VariableDeclaration {
    public:
        GlobalVariableDeclaration(VariableDeclaration::VarType vartype, Token* name, Type* type) : VariableDeclaration(vartype, name, type) {}
        GlobalVariableDeclaration(VariableDeclaration::VarType vartype, Token* name, Expression* initval) : VariableDeclaration(vartype, name, initval) {}
        GlobalVariableDeclaration(VariableDeclaration::VarType vartype, Token* name, Type* type, Expression* initval) : VariableDeclaration(vartype, name, type, initval) {}
        
        virtual void Accept(Visitor* v);
        
    };
    
    typedef std::list<VariableDeclaration*> VariableDeclarationList;
    
    class FunctionDeclaration : public ValueDeclaration, public HierarchicalConcreteDeclarationContext {
    public:
        FunctionDeclaration(Token* name, VariableDeclarationList* a, Type* rettype) : ValueDeclaration(name),
            val(NULL), body(NULL), functype(NULL), returntype(rettype), evaluatingtype(false)
            {
                if (a != NULL) {
                    std::copy(a->begin(), a->end(), std::back_inserter(this->args));
                    delete a;
                }
            }
        virtual void Accept(Visitor* v);
        
        const Ides::Types::Type* GetReturnType(ASTContext& ctx);
        virtual const Ides::Types::Type* GetType(ASTContext& ctx);
        
        const VariableDeclarationList& GetArgs() const { return args; }
        
        bool isVarArgs;
        Expression* val;
        Statement* body;
    private:
        const Ides::Types::FunctionType* functype;
        
        VariableDeclarationList args;
        
        Type* returntype;
        bool evaluatingtype;
    };
    
    class OverloadedFunction : public Declaration, public std::vector<Declaration*> {
    public:
        virtual const Ides::Types::Type* GetType(ASTContext& ctx) { return Ides::Types::OverloadedFunctionType::GetSingletonPtr(); }
        virtual void Accept(Visitor* v);
        
    };
    
    class TypeDeclaration : public NamedDeclaration {
    public:
        TypeDeclaration(Token* tok) : NamedDeclaration(tok) { }
        
    };
    
    class StructDeclaration : public TypeDeclaration {
    public:
        StructDeclaration(Token* name, DeclarationList* members) : TypeDeclaration(name)
        {
            if (members != NULL) {
                std::copy(members->begin(), members->end(), std::back_inserter(this->members));
                delete members;
            }
        }
        virtual void Accept(Visitor* v);
        
        virtual const Ides::Types::Type* GetType(ASTContext& ctx) {
            return Ides::Types::StructType::GetOrCreate(ctx, this->GetName());
        }
        
        DeclarationList members;
    };
    
    class FieldDeclaration : public VariableDeclaration {
    public:
        FieldDeclaration(VarType vartype, Token* name, Type* type) :
            VariableDeclaration(vartype, name, type) {}
        
        FieldDeclaration(VarType vartype, Token* name, Expression* initval) :
            VariableDeclaration(vartype, name, initval) {}
        
        FieldDeclaration(VarType vartype, Token* name, Type* type, Expression* initval) :
            VariableDeclaration(vartype, name, initval) {}
        
        virtual void Accept(Visitor* v);
    };
    
    class Namespace : public NamedDeclaration, public DeclarationContext {
        
    };
    
}
}

#endif /* defined(__ides__Declaration__) */
