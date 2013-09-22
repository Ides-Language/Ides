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
#include <ides/AST/DeclarationContext.h>
#include <ides/AST/AST.h>

namespace Ides {
namespace AST {
    
    class Attribute : public AST {
    public:
        Attribute(Token* tok, ExpressionList* args) : identifier(tok) {
            std::copy(args->begin(), args->end(), std::back_inserter(this->args));
        }
        
        Attribute(Token* tok) : identifier(tok) { }
        virtual void Accept(Visitor* v);
        
        Ides::StringRef GetName() const { return **identifier; }
        const ExpressionList& GetArgs() const { return args; }
    private:
        boost::scoped_ptr<Token> identifier;
        ExpressionList args;
    };
    
    typedef std::vector<Attribute*> AttributeList;
    
    class Declaration : public AST {
    public:
        virtual const Ides::Types::Type* GetType(ASTContext& ctx) = 0;
        
        Attribute* GetAttribute(Ides::StringRef str) const {
            auto i = attributes.find(str);
            if (i == attributes.end()) return NULL;
            return i->second;
        }
        
        void SetAttributes(AttributeList* attrs) {
            for (auto i = attrs->begin(); i != attrs->end(); ++i) {
                attributes.insert(std::make_pair((*i)->GetName(), *i));
            }
        }
    private:
        boost::unordered_map<Ides::String, Attribute*> attributes;
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
    
    class ArgumentDeclaration : public VariableDeclaration {
    public:
        ArgumentDeclaration(VariableDeclaration::VarType vartype, Token* name, Type* type) : VariableDeclaration(vartype, name, type) {}
        ArgumentDeclaration(VariableDeclaration::VarType vartype, Token* name, Expression* initval) : VariableDeclaration(vartype, name, initval) {}
        ArgumentDeclaration(VariableDeclaration::VarType vartype, Token* name, Type* type, Expression* initval) : VariableDeclaration(vartype, name, type, initval) {}
        
        virtual void Accept(Visitor* v);
        int argNum;
    };
    
    class GlobalVariableDeclaration : public VariableDeclaration {
    public:
        GlobalVariableDeclaration(VariableDeclaration::VarType vartype, Token* name, Type* type) : VariableDeclaration(vartype, name, type) {}
        GlobalVariableDeclaration(VariableDeclaration::VarType vartype, Token* name, Expression* initval) : VariableDeclaration(vartype, name, initval) {}
        GlobalVariableDeclaration(VariableDeclaration::VarType vartype, Token* name, Type* type, Expression* initval) : VariableDeclaration(vartype, name, type, initval) {}
        
        virtual void Accept(Visitor* v);
        
    };
    
    class FieldDeclaration : public VariableDeclaration {
    public:
        FieldDeclaration(VariableDeclaration::VarType vartype, Token* name, Type* type) : VariableDeclaration(vartype, name, type) {}
        FieldDeclaration(VariableDeclaration::VarType vartype, Token* name, Expression* initval) : VariableDeclaration(vartype, name, initval) {}
        FieldDeclaration(VariableDeclaration::VarType vartype, Token* name, Type* type, Expression* initval) : VariableDeclaration(vartype, name, type, initval) {}
        
        virtual void Accept(Visitor* v);
    };
    
    typedef std::list<VariableDeclaration*> VariableDeclarationList;
    typedef std::list<ArgumentDeclaration*> ArgumentDeclarationList;
    
    class FunctionDeclaration : public ValueDeclaration, public HierarchicalConcreteDeclarationContext {
    public:
        FunctionDeclaration(Token* name, ArgumentDeclarationList* a, Type* rettype) : ValueDeclaration(name),
            val(NULL), body(NULL), functype(NULL), returntype(rettype), evaluatingtype(false)
            {
                if (a != NULL) {
                    int arg = 0;
                    for (auto i = a->begin(); i != a->end(); ++i) {
                        this->args.push_back(*i);
                        (*i)->argNum = arg++;
                    }
                    delete a;
                }
            }
        virtual void Accept(Visitor* v);
        
        const Ides::Types::Type* GetReturnType(ASTContext& ctx);
        virtual const Ides::Types::Type* GetType(ASTContext& ctx);
        
        const ArgumentDeclarationList& GetArgs() const { return args; }
        
        Ides::String GetMangledName() const;
        
        bool isVarArgs;
        Expression* val;
        Statement* body;
    private:
        const Ides::Types::FunctionType* functype;
        
        ArgumentDeclarationList args;
        
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
        StructDeclaration(Token* name, DeclarationList* members) : TypeDeclaration(name), st(NULL)
        {
            if (members != NULL) {
                std::copy(members->begin(), members->end(), std::back_inserter(this->members));
                delete members;
            }
        }
        virtual void Accept(Visitor* v);
        
        virtual const Ides::Types::Type* GetType(ASTContext& ctx) {
            if (st != NULL) return st;
            st = Ides::Types::StructType::GetOrCreate(ctx, this->GetName());
            
            std::vector<std::pair<Ides::String, const Ides::Types::Type*> > membertypes;
            for (auto i = this->members.begin(); i != this->members.end(); ++i) {
                Ides::AST::NamedDeclaration* decl = (Ides::AST::NamedDeclaration*)*i;
                const Ides::Types::Type* memberType = decl->GetType(ctx);
                membertypes.push_back(std::make_pair(decl->GetName(), memberType));
                st->AddInstanceMember(decl->GetName(), decl);
            }
            st->SetMembers(ctx, membertypes);
            return st;
        }
        
        DeclarationList members;
    private:
        Ides::Types::StructType* st;
    };
    
    class Namespace : public NamedDeclaration, public DeclarationContext {
        
    };
    
}
}

#endif /* defined(__ides__Declaration__) */
