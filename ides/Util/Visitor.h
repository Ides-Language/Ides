//
//  Visitor.h
//  ides
//
//  Created by Sean Edwards on 1/11/14.
//
//

#ifndef ides_Visitor_h
#define ides_Visitor_h

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

#define IDES_FORM_ARGS(r, d, i, t) BOOST_PP_COMMA() t BOOST_PP_CAT(d, i)
#define IDES_ARGNAMES(...) BOOST_PP_LIST_FOR_EACH_I(IDES_FORM_ARGS, arg, BOOST_PP_VARIADIC_TO_LIST(__VA_ARGS__))

#define IDES_REF_VISITOR(r, d, t) (BOOST_PP_TUPLE_ELEM(4, 0, d) (*)\
    (const BOOST_PP_TUPLE_ELEM(4, 1, d) &, BOOST_PP_LIST_ENUM(BOOST_PP_TUPLE_ELEM(4, 3, d))))(& BOOST_PP_TUPLE_ELEM(4, 2, d) < t >) ,

#define IDES_FWD_DECL_VISITOR(r, d, t) \
    template<> BOOST_PP_TUPLE_ELEM(4, 0, d) BOOST_PP_TUPLE_ELEM(4, 2, d) < t > \
    (const t &, BOOST_PP_LIST_ENUM(BOOST_PP_TUPLE_ELEM(4, 3, d)));

#define DECL_VISITOR(types, basetype, name, ret, ...) \
    template<typename T> ret name (const T&, __VA_ARGS__ ); \
    BOOST_PP_SEQ_FOR_EACH(IDES_FWD_DECL_VISITOR, \
        (ret, basetype, name, BOOST_PP_VARIADIC_TO_LIST(__VA_ARGS__)), \
        types) \
    ret BOOST_PP_CAT(Do, name)(const basetype& val IDES_ARGNAMES(__VA_ARGS__)) { \
        static ret (*funcs[])(const basetype& val, __VA_ARGS__) = { \
            BOOST_PP_SEQ_FOR_EACH(IDES_REF_VISITOR, \
                (ret, basetype, name, BOOST_PP_VARIADIC_TO_LIST(__VA_ARGS__)), \
                types) \
            };  \
        return (*funcs[val.getKind()])(val, BOOST_PP_ENUM_PARAMS(BOOST_PP_VARIADIC_SIZE(__VA_ARGS__), arg)); \
    }

#define IDES_KIND_ENUM_ITEM(r, data, elem) BOOST_PP_CAT(data, elem),

#define DECL_TRAITS(types, traitname) \
    enum BOOST_PP_CAT(traitname, Kind) { \
    BOOST_PP_SEQ_FOR_EACH(IDES_KIND_ENUM_ITEM, BOOST_PP_CAT(traitname, Kind_), AST_TYPES) \
    BOOST_PP_CAT(traitname, Kind_LAST) \
}; \
template<typename T> struct BOOST_PP_CAT(traitname, Traits) { \
    static const BOOST_PP_CAT(traitname, Kind) kind; \
    static const char* name; \
    virtual BOOST_PP_CAT(traitname, Kind) getKind() const { return BOOST_PP_CAT(traitname, Traits)<T>::kind; } \
    virtual const char* getName() const { return BOOST_PP_CAT(traitname, Traits)<T>::name; } \
};

#define TRAITS_FWDDECL(r, traitname, t) struct t;
#define TRAITS_KINDDECL(r, traitname, t) template<> const BOOST_PP_CAT(traitname, Kind) \
    BOOST_PP_CAT(traitname, Traits) < t >::kind = BOOST_PP_CAT(BOOST_PP_CAT(traitname, Kind_), t);
#define TRAITS_NAMEDECL(r, traitname, t) template<> const char* BOOST_PP_CAT(traitname, Traits) < t >::name = BOOST_PP_STRINGIZE(t);

#define DEF_TRAITS(types, name) \
BOOST_PP_SEQ_FOR_EACH(TRAITS_FWDDECL, name, types) \
BOOST_PP_SEQ_FOR_EACH(TRAITS_KINDDECL, name, types) \
BOOST_PP_SEQ_FOR_EACH(TRAITS_NAMEDECL, name, types)


#endif
