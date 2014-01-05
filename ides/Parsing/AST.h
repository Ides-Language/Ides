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
#include <ides/Lang.h>

namespace Ides {
    using namespace Ides::Util;

    typedef const Ides::String Identifier;

    struct Ast {
        //Tree<Ides::SourceRange>::One source;
        Ast(const char* type) : type(type) {
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

        void Emit(YAML::Emitter& o);

        static size_t count;
        const char* type;
    private:
        virtual void DoEmit(YAML::Emitter& o) = 0;
    };

    struct Expr : Ast {
        Expr(const char* type) : Ast(type) {}
    };

    struct IdentifierExpr : Expr {
        IdentifierExpr(const Identifier& ident) : Expr("ident"), ident(ident) {}
        virtual ~IdentifierExpr() {}
        Identifier ident;

    private:
        virtual void DoEmit(YAML::Emitter& o);
    };

    struct DotExpr : Expr {
        DotExpr(Expr* lhs, IdentifierExpr* ident) : Expr("dot"), lhs(lhs), ident(ident->ident) {
            delete ident;
        }
        Tree<Expr>::One lhs;
        Identifier ident;

    private:
        virtual void DoEmit(YAML::Emitter& o);
    };

    struct TupleExpr : Expr {
        TupleExpr() : Expr("tuple") {}

        Tree<Expr>::Many items;

        void Add(Expr* exp) {
            items.push_back(std::unique_ptr<Expr>(exp));
        }

    private:
        virtual void DoEmit(YAML::Emitter& o);
    };

    struct CallExpr : Expr {
        CallExpr(Expr* lhs, TupleExpr* args)
            : Expr("call"), lhs(lhs), args(args) {}
        Tree<Expr>::One lhs;
        Tree<TupleExpr>::One args;

    private:
        virtual void DoEmit(YAML::Emitter& o);
    };

    struct VarArgsExpr : Expr {
        VarArgsExpr(Expr* lhs) : Expr("varargs"), lhs(lhs) {}
        Tree<Expr>::One lhs;

    private:
        virtual void DoEmit(YAML::Emitter& o) {
            lhs->Emit(o);
        }
    };

    struct IndexExpr : Expr {
        IndexExpr(Expr* lhs, TupleExpr* args)
        : Expr("index"), lhs(lhs), args(args) {}
        Tree<Expr>::One lhs;
        Tree<TupleExpr>::One args;

    private:
        virtual void DoEmit(YAML::Emitter& o);
    };

    template<typename T>
    struct ConstantExpr : Expr {
        ConstantExpr(const T& val) : Expr("constant"), value(val) {}
        T value;

    private:
        virtual void DoEmit(YAML::Emitter& o) {
            o << value;
        }
    };

    typedef ConstantExpr<std::string> ConstantString;
    typedef ConstantExpr<char> ConstantChar;
    typedef ConstantExpr<uint64_t> ConstantInt;
    typedef ConstantExpr<double> ConstantDec;
    typedef ConstantExpr<bool> ConstantBool;
    typedef ConstantExpr<uint8_t> PlaceholderExpr;

    struct InfixExpr : Expr {

        static InfixExpr* Create(IdentifierExpr* ident, Expr* lhs, Expr* rhs);

        Identifier ident;

        Tree<Expr>::One lhs;
        Tree<Expr>::One rhs;

    private:
        InfixExpr(IdentifierExpr* ident, Expr* lhs, Expr* rhs)
        : Expr("infix"), ident(ident->ident), lhs(lhs), rhs(rhs) {
            delete ident;
        }

        virtual void DoEmit(YAML::Emitter& o);
    };

    struct PrefixExpr : Expr {
        PrefixExpr(Expr* rhs, IdentifierExpr* ident) : Expr("prefix"), rhs(rhs), ident(ident->ident) {
            delete ident;
        }
        Tree<Expr>::One rhs;
        Identifier ident;

    private:
        virtual void DoEmit(YAML::Emitter& o);
    };

    struct IfExpr : Expr {
        IfExpr(Expr* cond, Expr* body) : Expr("if"), cond(cond), body(body) { }
        Tree<Expr>::One cond;
        Tree<Expr>::One body;

    private:
        virtual void DoEmit(YAML::Emitter& o);
    };

    struct BlockExpr : Expr {
        BlockExpr(Expr* lhs, TupleExpr* body) : Expr("block"), lhs(lhs), body(body) { }
        Tree<Expr>::One lhs;
        Tree<TupleExpr>::One body;

    private:
        virtual void DoEmit(YAML::Emitter& o);
    };

    struct Decl : public Expr {
        Decl(const char* type) : Expr(type) {}
    };

    struct Name : IdentifierExpr {
        Name(const Identifier& ident) : IdentifierExpr(ident), genericArgs(new TupleExpr()) {}
        Name(IdentifierExpr* ident) : IdentifierExpr(ident->ident), genericArgs(new TupleExpr()) {
            delete ident;
        }

        Name(IdentifierExpr* ident, TupleExpr* genericArgs) : IdentifierExpr(ident->ident), genericArgs(genericArgs) {
            delete ident;
        }
        virtual ~Name() {}

        Tree<TupleExpr>::One genericArgs;

    private:
        virtual void DoEmit(YAML::Emitter& o);
    };

    template<typename DeclType, DataKind Kind = NONE, typename IdentKind = Name>
    struct NamedDecl : Decl {
        NamedDecl(Visibility vis, IdentKind* name, DeclType* decl) :
            Decl("decl"), name(name), decl(decl), vis(vis)
        {
        }
        typename Tree<IdentKind>::One name;
        typename Tree<DeclType>::One decl;
        Visibility vis;

    private:
        virtual void DoEmit(YAML::Emitter& o) {
            o << YAML::BeginMap;
            o << YAML::Key << "name" << YAML::Value;
            name->Emit(o);
            o << YAML::Key << "decl" << YAML::Value;
            decl->Emit(o);
            o << YAML::EndMap;
        }
    };

    struct DataStructureDecl : Decl {
        DataStructureDecl(TupleExpr* args, TupleExpr* supers, TupleExpr* body) : Decl("struct"), args(args), superTypes(supers), body(body) { }

        Tree<TupleExpr>::One args;
        Tree<TupleExpr>::One superTypes;
        Tree<TupleExpr>::One body;

    private:
        virtual void DoEmit(YAML::Emitter& o);
    };

    struct FunctionDecl : Decl {
        FunctionDecl(TupleExpr* args, Expr* type, Expr* body) : Decl("def"), args(args), type(type), body(body) { }

        Tree<TupleExpr>::One args;
        Tree<Expr>::One type;
        Tree<Expr>::One body;

    private:
        virtual void DoEmit(YAML::Emitter& o);
    };

    template<ValKind Kind>
    struct ValueDecl : Decl {
        ValueDecl(Expr* type, Expr* init) : Decl(Kind == VAL ? "val" : "var"),
            type(type), init(init) {}
        Tree<Expr>::One type;
        Tree<Expr>::One init;

    private:
        virtual void DoEmit(YAML::Emitter& o) {
            o << YAML::BeginMap;
            o << YAML::Key << "type" << YAML::Value;
            type->Emit(o);
            o << YAML::Key << "init" << YAML::Value;
            init->Emit(o);
        }
    };

    typedef NamedDecl<DataStructureDecl, TRAIT> TraitDecl;
    typedef NamedDecl<DataStructureDecl, CLASS> ClassDecl;
    typedef NamedDecl<DataStructureDecl, STRUCT> StructDecl;

    typedef NamedDecl<ValueDecl<VAL>> ValDecl;
    typedef NamedDecl<ValueDecl<VAR>> VarDecl;

    typedef NamedDecl<FunctionDecl> FnDecl;
    typedef NamedDecl<Expr, NONE, IdentifierExpr> ArgDecl;

    typedef NamedDecl<TupleExpr> ModuleDecl;

    struct CasePair : public Decl {
        CasePair(Expr* lhs, Expr* rhs) : Decl("pair"), lhs(lhs), rhs(rhs) {}

        Tree<Expr>::One lhs;
        Tree<Expr>::One rhs;

    private:
        virtual void DoEmit(YAML::Emitter& o);
    };

    struct PartialFunction : public Expr {
        PartialFunction() : Expr("partialfn") {}
        Tree<CasePair>::Many cases;

        void Add(CasePair* exp) {
            cases.push_back(Tree<CasePair>::One(exp));
        }

    private:
        virtual void DoEmit(YAML::Emitter& o);
    };

}

std::ostream& operator<<(std::ostream& os, const Ides::Ast& data);

#endif
