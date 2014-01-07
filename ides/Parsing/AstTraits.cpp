//
//  AstTraits.cpp
//  ides
//
//  Created by Sean Edwards on 1/6/14.
//
//

#include <ides/Parsing/AstTraits.h>

namespace Ides {

#define AST_FWDDECL(r, d, t) struct t;
#define AST_KINDDECL(r, d, t) template<> const AstKind AstTraits< Ides:: t >::kind = BOOST_PP_CAT(Ides::Ast_, t);
#define AST_NAMEDECL(r, d, t) template<> const char* AstTraits< Ides:: t >::name = BOOST_PP_STRINGIZE(t);

    BOOST_PP_SEQ_FOR_EACH(AST_FWDDECL, _, AST_TYPES);
    BOOST_PP_SEQ_FOR_EACH(AST_KINDDECL, _, AST_TYPES);
    BOOST_PP_SEQ_FOR_EACH(AST_NAMEDECL, _, AST_TYPES);


        //DECL_AST_VISITOR(AST_TYPES, PrintNode, void, std::ostream&, size_t);
}