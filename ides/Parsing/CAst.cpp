//
//  CAst.cpp
//  ides
//
//  Created by Sean Edwards on 1/5/14.
//
//

#include <ides/common.h>
#include <ides/Parsing/AST.h>
#include <ides/Parsing/CAst.h>

#define AST_AS(T, ast) ((const T*)(ast))

extern "C" {

    const char* get_ast_type(const Ast* ast) {
        return AST_AS(Ides::AstBase, ast)->getName();
    }


    const char* get_ident_ident(const IdentifierExpr* expr) {
        return AST_AS(Ides::IdentifierExpr, expr)->ident.c_str();
    }

    const Expr* get_tuple_item(const ExprList* expr, unsigned int item) {
        return AST_AS(Expr, AST_AS(Ides::ExprList, expr)->items[item].get());

    }
    unsigned int get_tuple_size(const ExprList* expr) {
        return AST_AS(Ides::ExprList, expr)->items.size();
    }


#define VISIT_CASE(ctype) \
    if (ast != 0 && strcmp(get_ast_type(ast), Ides::AstTraits<Ides::ctype>::name) == 0 && visitor->visit_##ctype != 0) { \
        ret = AST_AS(Ast, (*visitor->visit_##ctype)(visitor, AST_AS(ctype, ast))); \
    }

    const Ast* visit(struct CAstVisitor* visitor, const Ast* ast) {
        const Ast* ret = ast;
        VISIT_CASE(ExprList)
        VISIT_CASE(CallExpr)
        return ret;
    }

}
