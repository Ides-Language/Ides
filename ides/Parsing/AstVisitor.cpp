//
//  AstVisitor.cpp
//  ides
//
//  Created by Sean Edwards on 1/8/14.
//
//

#include "AstVisitor.h"

namespace {
    DECL_AST_VISITOR(VisitAstVisitor, void, Ides::AstVisitor*);


#define IDES_AST_VISITOR_MEMBER_IMPL(r, data, elem) template<> \
    void VisitAstVisitor(const Ides::elem & ast, Ides::AstVisitor* visitor) { return visitor->Visit(ast); }
    BOOST_PP_SEQ_FOR_EACH(IDES_AST_VISITOR_MEMBER_IMPL, _, AST_TYPES)

}

namespace Ides {
    void AstVisitor::DoVisit(const Ides::AstBase& ast) { DoVisitAstVisitor(ast, this); }
}
