//
//  CAst.h
//  ides
//
//  Created by Sean Edwards on 1/5/14.
//
//

#ifndef ides_CAst_h
#define ides_CAst_h

extern "C" {
    struct Ast;
    const char* get_ast_type(const Ast* ast);
    Ast* clone_ast(const Ast* ast);

    typedef struct Expr Expr;
    struct IdentifierExpr;

    const char* get_ident_ident(const IdentifierExpr* expr);
    IdentifierExpr* clone_ident(const IdentifierExpr* other);

    typedef struct DotExpr DotExpr;
    const Expr* get_dot_lhs(const DotExpr* expr);
    const char* get_dot_ident(const DotExpr* expr);

    typedef struct ExprList ExprList;
    const Expr* get_tuple_item(const ExprList* expr, unsigned int item);
    unsigned int get_tuple_size(const ExprList* expr);

    typedef struct CallExpr CallExpr;
    const Expr* get_call_lhs(const CallExpr* expr);
    const ExprList* get_call_args(const CallExpr* expr);

    typedef struct VarArgsExpr VarArgsExpr;
    const Expr* get_varargs_lhs(const VarArgsExpr* expr);

    typedef struct IndexExpr IndexExpr;
    const Expr* get_index_lhs(const IndexExpr* expr);
    const ExprList* get_index_args(const IndexExpr* expr);

    typedef struct ConstantString ConstantString;
    const char* get_conststr_data(const ConstantString* expr);

    typedef struct ConstantChar ConstantChar;
    unsigned char get_constchar_data(const ConstantChar* expr);

    typedef struct ConstantInt ConstantInt;
    unsigned long long int get_constint_data(const ConstantInt* expr);

    typedef struct ConstantDec ConstantDec;
    double get_constdec_data(const ConstantDec* expr);

    typedef struct ConstantBool ConstantBool;
    int get_constbool_data(const ConstantBool* expr);

    typedef struct PlaceholderExpr PlaceholderExpr;
    unsigned int get_placeholder_id(const PlaceholderExpr* expr);

    typedef struct InfixExpr InfixExpr;
    const Expr* get_infix_lhs(const InfixExpr* expr);
    const Expr* get_infix_rhs(const InfixExpr* expr);
    const char* get_infix_ident(const InfixExpr* expr);

    typedef struct PrefixExpr PrefixExpr;
    const Expr* get_prefix_rhs(const PrefixExpr* expr);
    const char* get_prefix_ident(const PrefixExpr* expr);

    typedef struct IfExpr IfExpr;
    const Expr* get_if_cond(const IfExpr* expr);
    const Expr* get_if_body(const IfExpr* expr);

    typedef struct BlockExpr BlockExpr;
    const Expr* get_block_lhs(const BlockExpr* expr);
    const Expr* get_block_body(const BlockExpr* expr);

    typedef struct Decl Decl;
    typedef struct Name Name;
    const ExprList* get_name_typeargs(const Name* expr);

    typedef struct NamedDecl NamedDecl;
    typedef struct DataStructureDecl DataStructureDecl;

    typedef struct FunctionDecl FunctionDecl;
    const ExprList* get_function_args(const FunctionDecl* expr);
    const Expr* get_function_rettype(const FunctionDecl* expr);
    const Expr* get_function_body(const FunctionDecl* expr);

    typedef struct PartialFunction PartialFunction;
    unsigned int get_partialfunction_case_count(const PartialFunction* expr);
    const Expr* get_partialfunction_case_lhs(const PartialFunction* expr, unsigned int idx);
    const Expr* get_partialfunction_case_rhs(const PartialFunction* expr, unsigned int idx);

#define VISIT_DECL(type) const type* (*visit_##type)(struct CAstVisitor* visitor, const type* expr)
    struct CAstVisitor {
        VISIT_DECL(DotExpr);
        VISIT_DECL(ExprList);
        VISIT_DECL(CallExpr);
        VISIT_DECL(IdentifierExpr);

        void* userdata;
    };

    const Ast* visit(struct CAstVisitor* visitor, const Ast* ast);

    /*

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
        DataStructureDecl(ExprList* args, ExprList* supers, ExprList* body) : Decl("struct"), args(args), superTypes(supers), body(body) { }

        Tree<ExprList>::One args;
        Tree<ExprList>::One superTypes;
        Tree<ExprList>::One body;

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

    typedef NamedDecl<ExprList> ModuleDecl;*/
}

#endif
