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
#include <ides/Parsing/AstTraits.h>

namespace Ides {
    struct AstBase;
}

std::ostream& operator<<(std::ostream& os, const Ides::AstBase& data);



namespace Ides {
    using namespace Ides::Util;

    typedef const Ides::String Identifier;


    struct AstBase {
        Ides::SourceRange source;

        template<typename T>
        T* As() {
            return dynamic_cast<T*>(this);
        }

        template<typename T>
        const T* As() const {
            return dynamic_cast<const T*>(this);
        }

        void Emit(YAML::Emitter& o);

        Ides::String ToString() const {
            std::ostringstream str;
            str << *this;
            return str.str();
        }

        static AstBase* Read(const YAML::Node& n);
        template<typename RT>
        static RT* Read(const YAML::Node& n) { return Read(n)->template As<RT>(); }

        virtual AstKind getKind() const = 0;
        virtual const char* getName() const = 0;
    private:
        virtual void DoEmit(YAML::Emitter& o) = 0;
    };

    typedef AstBase Ast;

    typedef AstBase Expr;
    typedef AstBase Decl;

    template<typename T>
    struct AstImpl : AstBase, AstTraits<T> {
        virtual AstKind getKind() const { return AstTraits<T>::getKind(); }
        virtual const char* getName() const { return AstTraits<T>::getName(); }
    };

    struct IdentifierExpr : AstImpl<IdentifierExpr> {
        IdentifierExpr(const Identifier& ident) : ident(ident) {}
        virtual ~IdentifierExpr() {}
        Identifier ident;

    private:
        virtual void DoEmit(YAML::Emitter& o);
    };

    struct ExprList : AstImpl<ExprList>, Tree<Expr>::Many {
        ExprList() : items(*this) { }

        Tree<Expr>::Many& items;

        void Add(Expr* exp) {
            items.push_back(Tree<Expr>::One(exp));
        }

    private:
        virtual void DoEmit(YAML::Emitter& o);
    };

    struct Name : AstImpl<Name> {
        Name(const Identifier& ident) : ident(new IdentifierExpr(ident)), genericArgs(new ExprList()) {}
        Name(IdentifierExpr* ident) : ident(ident), genericArgs(new ExprList()) { }
        Name(IdentifierExpr* ident, ExprList* genericArgs) : ident(ident), genericArgs(genericArgs) { }
        virtual ~Name() {}

        Tree<IdentifierExpr>::One ident;
        Tree<ExprList>::One genericArgs;

    private:
        virtual void DoEmit(YAML::Emitter& o);
    };

    struct CallExpr : AstImpl<CallExpr> {
        CallExpr(Expr* lhs, ExprList* args)
            : lhs(lhs), args(args) {}
        Tree<Expr>::One lhs;
        Tree<ExprList>::One args;

    private:
        virtual void DoEmit(YAML::Emitter& o);
    };

    struct IndexExpr : AstImpl<IndexExpr> {
        IndexExpr(Expr* lhs, ExprList* args)
        : lhs(lhs), args(args) {}
        Tree<Expr>::One lhs;
        Tree<ExprList>::One args;

    private:
        virtual void DoEmit(YAML::Emitter& o);
    };

    typedef uint16_t placeholder_t;
    typedef boost::variant<std::string, uint64_t, placeholder_t, double, bool, char> ConstantValue;
    struct ConstantExpr : AstImpl<ConstantExpr> {
        ConstantExpr(const ConstantValue& val) : value(val) {}
        ConstantValue value;

    private:
        virtual void DoEmit(YAML::Emitter& o) {
            o << (std::string)(StringBuilder() << value);
        }
    };

    typedef ConstantExpr ConstantInt;
    typedef ConstantExpr ConstantDec;
    typedef ConstantExpr ConstantBool;
    typedef ConstantExpr ConstantString;
    typedef ConstantExpr ConstantChar;
    typedef ConstantExpr PlaceholderExpr;

    struct BinaryExpr : AstImpl<BinaryExpr> {
        BinaryExpr(IdentifierExpr* fn, Expr* lhs, Expr* rhs)
        : fn(fn), lhs(lhs), rhs(rhs) { }

        static BinaryExpr* Create(IdentifierExpr* ident, Expr* lhs, Expr* rhs);

        Tree<IdentifierExpr>::One fn;

        Tree<Expr>::One lhs;
        Tree<Expr>::One rhs;

    private:

        virtual void DoEmit(YAML::Emitter& o);
    };

    struct UnaryExpr : AstImpl<UnaryExpr> {
        UnaryExpr(Expr* fn, Expr* arg, bool is_prefix = true)
            : fn(fn), arg(arg), is_prefix(is_prefix) { }
        Tree<Expr>::One fn;
        Tree<Expr>::One arg;
        bool is_prefix;

    private:
        virtual void DoEmit(YAML::Emitter& o);
    };

    typedef std::pair<Tree<Expr>::One, Tree<Expr>::One> CasePair;

    struct PartialFunction : AstImpl<PartialFunction>, std::vector<CasePair> {

        void Add(Expr* lhs, Expr* rhs) {
            push_back(CasePair(Tree<Expr>::One(lhs), Tree<Expr>::One(rhs)));
        }

    private:
        virtual void DoEmit(YAML::Emitter& o);
    };

    template<typename Self, typename DeclType, typename IdentKind = Name>
    struct NamedDecl : AstImpl<Self> {
        NamedDecl(Visibility vis, IdentKind* name, DeclType* decl)
            : name(name), decl(decl), vis(vis) {
                assert(decl != NULL);
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

    struct DataStructureDecl : AstImpl<DataStructureDecl> {
        DataStructureDecl(ExprList* args, ExprList* supers, ExprList* body)
            : args(args), superTypes(supers), body(body) { }

        Tree<ExprList>::One args;
        Tree<ExprList>::One superTypes;
        Tree<ExprList>::One body;

    private:
        virtual void DoEmit(YAML::Emitter& o);
    };

    struct FnDataDecl : AstImpl<FnDataDecl> {
        FnDataDecl(ExprList* args, Expr* type, Expr* body)
            : args(args), type(type), body(body) { }

        Tree<ExprList>::One args;
        Tree<Expr>::One type;
        Tree<Expr>::One body;

    private:
        virtual void DoEmit(YAML::Emitter& o);
    };

    struct ValueDecl : AstImpl<ValueDecl> {
        ValueDecl(Expr* type, Expr* init) : type(type), init(init) {}
        Tree<Expr>::One type;
        Tree<Expr>::One init;

    private:
        virtual void DoEmit(YAML::Emitter& o) {
            o << YAML::BeginMap;
            if (type) {
                o << YAML::Key << "type" << YAML::Value;
                type->Emit(o);
            }
            if (init) {
                o << YAML::Key << "init" << YAML::Value;
                init->Emit(o);
            }
            o << YAML::EndMap;
        }
    };

    struct TraitDecl : NamedDecl<TraitDecl, DataStructureDecl> {
        TraitDecl(Visibility vis, Name* name, DataStructureDecl* decl) : NamedDecl(vis, name, decl) { }
    };
    struct ClassDecl : NamedDecl<ClassDecl, DataStructureDecl> {
        ClassDecl(Visibility vis, Name* name, DataStructureDecl* decl) : NamedDecl(vis, name, decl) { }
    };
    struct StructDecl : NamedDecl<StructDecl, DataStructureDecl> {
        StructDecl(Visibility vis, Name* name, DataStructureDecl* decl) : NamedDecl(vis, name, decl) { }
    };

    struct ValDecl : NamedDecl<ValDecl, ValueDecl> {
        ValDecl(Visibility vis, Name* name, ValueDecl* decl) : NamedDecl(vis, name, decl) { }
    };
    struct VarDecl : NamedDecl<VarDecl, ValueDecl> {
        VarDecl(Visibility vis, Name* name, ValueDecl* decl) : NamedDecl(vis, name, decl) { }
    };
    struct FnDecl : NamedDecl<FnDecl, FnDataDecl> {
        FnDecl(Visibility vis, Name* name, FnDataDecl* decl) : NamedDecl(vis, name, decl) { }
    };
    struct ArgDecl : NamedDecl<ArgDecl, Expr, IdentifierExpr> {
        ArgDecl(Visibility vis, IdentifierExpr* name, Expr* decl) : NamedDecl(vis, name, decl) { }
    };

    struct ModuleDecl : NamedDecl<ModuleDecl, ExprList> {
        ModuleDecl(Visibility vis, Name* name, ExprList* decl) : NamedDecl(vis, name, decl) { }
    };

}

#endif
