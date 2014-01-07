//
//  AstTraits.h
//  ides
//
//  Created by Sean Edwards on 1/6/14.
//
//

#ifndef ides_AstTraits_h
#define ides_AstTraits_h

#define BOOST_PP_VARIADICS 1

#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/preprocessor/list/for_each_i.hpp>
#include <boost/preprocessor/punctuation/comma.hpp>
#include <boost/preprocessor/tuple/elem.hpp>
#include <boost/preprocessor/tuple/enum.hpp>
#include <boost/preprocessor/seq/enum.hpp>
#include <boost/preprocessor/stringize.hpp>
#include <boost/preprocessor/variadic/to_list.hpp>
#include <boost/preprocessor/list/enum.hpp>
#include <boost/preprocessor/repetition/enum_params.hpp>
#include <boost/preprocessor/slot/counter.hpp>

#define AST_TYPES \
(IdentifierExpr)\
(ExprList)\
(CallExpr)\
(IndexExpr)\
(ConstantExpr)\
(BinaryExpr)\
(UnaryExpr)\
(Name)\
(TraitDecl)\
(ClassDecl)\
(StructDecl)\
(ValueDecl)\
(ValDecl)\
(VarDecl)\
(FnDataDecl)\
(FnDecl)\
(ArgDecl)\
(ModuleDecl)\
(PartialFunction)\
(DataStructureDecl)

#define IDES_AST_KIND_ENUM_ITEM(r, data, elem) BOOST_PP_CAT(data, elem),

namespace Ides {
    enum AstKind {
        BOOST_PP_SEQ_FOR_EACH(IDES_AST_KIND_ENUM_ITEM, Ast_, AST_TYPES)

        Ast_LAST
    };

    template<typename T>
    struct AstTraits {
        static const AstKind kind;
        static const char* name;

        virtual AstKind getKind() const { return AstTraits<T>::kind; }
        virtual const char* getName() const { return AstTraits<T>::name; }
    };
}

#define IDES_FORM_AST_ARGS(r, d, i, t) BOOST_PP_COMMA() t BOOST_PP_CAT(d, i)
#define IDES_AST_ARGNAMES(...) BOOST_PP_LIST_FOR_EACH_I(IDES_FORM_AST_ARGS, arg, BOOST_PP_VARIADIC_TO_LIST(__VA_ARGS__))

#define IDES_FWD_DECL_AST_VISITOR(r, d, t) \
    template<> BOOST_PP_TUPLE_ELEM(3, 0, d)\
    BOOST_PP_TUPLE_ELEM(3, 1, d) \
    (const Ides:: t & ast, BOOST_PP_LIST_ENUM(BOOST_PP_TUPLE_ELEM(3, 2, d)));

#define IDES_REF_AST_VISITOR(r, d, t) (BOOST_PP_TUPLE_ELEM(3, 0, d) (*)\
    (const Ides::AstBase &, BOOST_PP_LIST_ENUM(BOOST_PP_TUPLE_ELEM(3, 2, d))))(& BOOST_PP_TUPLE_ELEM(3, 1, d) < Ides:: t >) ,

#define DECL_AST_VISITOR(types, name, ret, ...) \
    template<typename T> ret name (const T&, __VA_ARGS__ ); \
    BOOST_PP_SEQ_FOR_EACH(IDES_FWD_DECL_AST_VISITOR, \
        (ret, name, BOOST_PP_VARIADIC_TO_LIST(__VA_ARGS__)), \
        types) \
    ret BOOST_PP_CAT(Do, name)(const Ides::AstBase& ast IDES_AST_ARGNAMES(__VA_ARGS__)) { \
        static ret (*funcs[])(const Ides::AstBase& ast, __VA_ARGS__) = { \
            BOOST_PP_SEQ_FOR_EACH(IDES_REF_AST_VISITOR, \
                (ret, name, BOOST_PP_VARIADIC_TO_LIST(__VA_ARGS__)), \
                AST_TYPES) \
        };  \
        (*funcs[ast.getKind()])(ast, BOOST_PP_ENUM_PARAMS(BOOST_PP_VARIADIC_SIZE(__VA_ARGS__), arg)); \
    }

#endif
