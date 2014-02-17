//
//  AstVisitor.h
//  ides
//
//  Created by Sean Edwards on 1/8/14.
//
//

#ifndef __ides__AstVisitor__
#define __ides__AstVisitor__

#include <iostream>

#include <ides/Parsing/AST.h>

namespace Ides {

    class AstVisitor {
    public:
        virtual ~AstVisitor() { }

        virtual void DoVisit(const Ides::AstBase& ast);

#define IDES_AST_VISITOR_MEMBER(r, data, elem) virtual void Visit(const elem & ast) { }
        BOOST_PP_SEQ_FOR_EACH(IDES_AST_VISITOR_MEMBER, _, AST_TYPES)
    };

    template<typename T>
    class ReturningAstVisitor : private AstVisitor {
    public:
        virtual ~ReturningAstVisitor() { }

        virtual T DoAccept(const Ides::AstBase& ast) {
            try {
                DoVisit(ast);
            } catch (...) {
                last = T();
                throw;
            }
            return last;
        }


#define IDES_AST_ACCEPTER_MEMBER(r, data, elem) virtual T Accept(const elem & ast) { return T(); }
        BOOST_PP_SEQ_FOR_EACH(IDES_AST_ACCEPTER_MEMBER, _, AST_TYPES)

    private:

#define IDES_AST_ACCEPTER_VISITOR_MEMBER(r, data, elem) virtual void Visit(const elem & ast) { last = Accept(ast); }
        BOOST_PP_SEQ_FOR_EACH(IDES_AST_ACCEPTER_VISITOR_MEMBER, _, AST_TYPES)

        T last;
    };
}

#endif /* defined(__ides__AstVisitor__) */
