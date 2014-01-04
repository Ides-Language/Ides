//
//  AST.h
//  ides
//
//  Created by Sean Edwards on 12/30/13.
//
//

#ifndef _IDES_AST_H_
#define _IDES_AST_H_

#include <ides/common.h>
#include <ides/Source/SourceLocation.h>

namespace Ides {
    using namespace Ides::Util;

    typedef const Ides::String Identifier;

    enum ValKind {
        VAL,
        VAR
    };

    enum DataKind {
        NONE,
        TRAIT,
        CLASS,
        STRUCT
    };

    enum Visibility {
        V_PUBLIC,
        V_PRIVATE,
        V_PROTECTED,
        V_INTERNAL,

        V_EXTERN = 1 << 10,
        V_CONST = 1 << 11,
        V_ABSTRACT = 1 << 12,

        V_DEFAULT = V_PRIVATE
    };

    struct Ast {
        //Tree<Ides::SourceRange>::One source;
        Ast() {
            ++count;
        }
        virtual ~Ast() {
            --count;
        }

        template<typename T>
        T* As() {
            return dynamic_cast<T*>(this);
        }

        template<typename T>
        const T* As() const {
            return dynamic_cast<const T*>(this);
        }

        static size_t count;
    };

    struct Expr : Ast {
        virtual ~Expr() {}
    };

    struct IdentifierExpr : Expr {
        IdentifierExpr(const Identifier& ident) : ident(ident) {}
        virtual ~IdentifierExpr() {}
        Identifier ident;
    };

    struct DotExpr : Expr {
        DotExpr(Expr* lhs, IdentifierExpr* ident) : lhs(lhs), ident(ident->ident) {
            delete ident;
        }
        Tree<Expr>::One lhs;
        Identifier ident;
    };

    struct TupleExpr : Expr {
        Tree<Expr>::Many items;

        void Add(Expr* exp) {
            items.push_back(std::unique_ptr<Expr>(exp));
        }
    };

    struct CallExpr : Expr {
        CallExpr(Expr* lhs, TupleExpr* args)
            : lhs(lhs), args(args) {}
        Tree<Expr>::One lhs;
        Tree<TupleExpr>::One args;
    };

    struct IndexExpr : Expr {
        IndexExpr(Expr* lhs, TupleExpr* args)
        : lhs(lhs), args(args) {}
        Tree<Expr>::One lhs;
        Tree<TupleExpr>::One args;
    };

    template<typename T>
    struct ConstantExpr : Expr {
        ConstantExpr(const T& val) : value(val) {}
        const T value;
    };

    typedef ConstantExpr<std::string> ConstantString;
    typedef ConstantExpr<char> ConstantChar;
    typedef ConstantExpr<uint64_t> ConstantInt;
    typedef ConstantExpr<double> ConstantDec;
    typedef ConstantExpr<bool> ConstantBool;

    struct InfixExpr : Expr {

        static InfixExpr* Create(IdentifierExpr* ident, Expr* lhs, Expr* rhs);

        Identifier ident;

        Tree<Expr>::One lhs;
        Tree<Expr>::One rhs;

    private:
        InfixExpr(IdentifierExpr* ident, Expr* lhs, Expr* rhs)
        : ident(ident->ident), lhs(lhs), rhs(rhs) {
            delete ident;
        }
    };

    struct Decl : public Expr {
    };

    template<typename DeclType, DataKind Kind = NONE>
    struct NamedDecl : Decl {
        NamedDecl(Visibility vis, IdentifierExpr* name, DeclType* decl) :
            name(name->ident), decl(decl), vis(vis)
        {
            delete name;
        }
        Ides::Identifier name;
        typename Tree<DeclType>::One decl;
        Visibility vis;
    };

    struct DataStructureDecl : Decl {
        DataStructureDecl(TupleExpr* supers, TupleExpr* body) : superTypes(supers), body(body) { }

        Tree<TupleExpr>::One superTypes;
        Tree<TupleExpr>::One body;

    };

    template<ValKind Kind>
    struct ValueDecl : Decl {
        ValueDecl(Visibility vis, Expr* type) :
            type(type) {}
        Tree<Expr>::One type;
    };

    typedef NamedDecl<DataStructureDecl, TRAIT> TraitDecl;
    typedef NamedDecl<DataStructureDecl, CLASS> ClassDecl;
    typedef NamedDecl<DataStructureDecl, STRUCT> StructDecl;

    typedef NamedDecl<ValueDecl<VAL>> ValDecl;
    typedef NamedDecl<ValueDecl<VAR>> VarDecl;

}

std::ostream& operator<<(std::ostream& os, const Ides::Ast& data);

#endif
