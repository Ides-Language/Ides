//
//  Declaration.h
//  ides
//
//  Created by Sean Edwards on 12/30/12.
//
//

#ifndef __ides__Declaration__
#define __ides__Declaration__


#include <ides/AST/AST.h>
#include <ides/AST/Expression.h>
#include <ides/AST/Statement.h>
#include <ides/AST/DeclarationContext.h>

namespace Ides {
namespace AST {
    
    class Declaration : public AST {
    public:
        virtual const Ides::Types::Type* GetType(ASTContext& ctx) = 0;
    };
    
    typedef std::list<boost::shared_ptr<Declaration> > DeclarationList;
    
    class NamedDeclaration : public Declaration {
    protected:
        NamedDeclaration(Token* tok) : name(tok) { }
    public:
        const Ides::String& GetName() const { return **name; }
    private:
        boost::scoped_ptr<Token> name;
    };
    
    class ValueDeclaration : public NamedDeclaration {
    protected:
        ValueDeclaration(Token* tok) : NamedDeclaration(tok) { }
    public:
        virtual void Accept(Visitor* v) { v->Visit(this); }
        
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
        
        virtual void Accept(Visitor* v) { v->Visit(this); }
        
        virtual const Ides::Types::Type* GetType(ASTContext& ctx) { return type->GetType(ctx); }
        
        VarType vartype;
        boost::scoped_ptr<Type> type;
        boost::scoped_ptr<Expression> initval;
    };
    
    typedef std::list<boost::shared_ptr<VariableDeclaration> > VariableDeclarationList;
    
    class FunctionDeclaration : public ValueDeclaration, public DeclarationContext {
    public:
        FunctionDeclaration(Token* name, VariableDeclarationList* a, Type* rettype) : ValueDeclaration(name),
            val(NULL), body(NULL), functype(NULL), returntype(rettype), evaluatingtype(false)
            {
                if (a != NULL) {
                    std::copy(a->begin(), a->end(), std::back_inserter(this->args));
                    delete a;
                }
            }
        virtual void Accept(Visitor* v) { v->Visit(this); }
        
        const Ides::Types::Type* GetReturnType(ASTContext& ctx);
        virtual const Ides::Types::Type* GetType(ASTContext& ctx);
        
        const VariableDeclarationList& GetArgs() const { return args; }
        
        Expression* val;
        Statement* body;
    private:
        const Ides::Types::FunctionType* functype;
        
        VariableDeclarationList args;
        
        boost::scoped_ptr<Type> returntype;
        bool evaluatingtype;
    };
    
    class StructDeclaration : public NamedDeclaration, public DeclarationContext {
    public:
        StructDeclaration(Token* name, DeclarationList* members) : NamedDeclaration(name)
        {
            if (members != NULL) {
                std::copy(members->begin(), members->end(), std::back_inserter(this->members));
                delete members;
            }
        }
        virtual void Accept(Visitor* v) { v->Visit(this); }
        
        virtual const Ides::Types::Type* GetType(ASTContext& ctx) {
            return Ides::Types::StructType::GetOrCreate(ctx, this->GetName());
        }
        
        void GenType(ASTContext& ctx);
        
        DeclarationList members;
    };
    
    class Namespace : public NamedDeclaration, public DeclarationContext {
        
    };
    
}
}

#endif /* defined(__ides__Declaration__) */
