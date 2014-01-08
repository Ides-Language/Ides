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
    template<> void VisitAstVisitor(const Ides::IdentifierExpr& ast, Ides::AstVisitor* visitor) { return visitor->Visit(ast); }
    template<> void VisitAstVisitor(const Ides::ExprList& ast, Ides::AstVisitor* visitor) { return visitor->Visit(ast); }
    template<> void VisitAstVisitor(const Ides::CallExpr& ast, Ides::AstVisitor* visitor) { return visitor->Visit(ast); }
    template<> void VisitAstVisitor(const Ides::IndexExpr& ast, Ides::AstVisitor* visitor) { return visitor->Visit(ast); }
    template<> void VisitAstVisitor(const Ides::ConstantExpr& ast, Ides::AstVisitor* visitor) { return visitor->Visit(ast); }
    template<> void VisitAstVisitor(const Ides::BinaryExpr& ast, Ides::AstVisitor* visitor) { return visitor->Visit(ast); }
    template<> void VisitAstVisitor(const Ides::UnaryExpr& ast, Ides::AstVisitor* visitor) { return visitor->Visit(ast); }
    template<> void VisitAstVisitor(const Ides::Name& ast, Ides::AstVisitor* visitor) { return visitor->Visit(ast); }
    template<> void VisitAstVisitor(const Ides::TraitDecl& ast, Ides::AstVisitor* visitor) { return visitor->Visit(ast); }
    template<> void VisitAstVisitor(const Ides::ClassDecl& ast, Ides::AstVisitor* visitor) { return visitor->Visit(ast); }
    template<> void VisitAstVisitor(const Ides::StructDecl& ast, Ides::AstVisitor* visitor) { return visitor->Visit(ast); }
    template<> void VisitAstVisitor(const Ides::ValueDecl& ast, Ides::AstVisitor* visitor) { return visitor->Visit(ast); }
    template<> void VisitAstVisitor(const Ides::ValDecl& ast, Ides::AstVisitor* visitor) { return visitor->Visit(ast); }
    template<> void VisitAstVisitor(const Ides::VarDecl& ast, Ides::AstVisitor* visitor) { return visitor->Visit(ast); }
    template<> void VisitAstVisitor(const Ides::FnDataDecl& ast, Ides::AstVisitor* visitor) { return visitor->Visit(ast); }
    template<> void VisitAstVisitor(const Ides::FnDecl& ast, Ides::AstVisitor* visitor) { return visitor->Visit(ast); }
    template<> void VisitAstVisitor(const Ides::ArgDecl& ast, Ides::AstVisitor* visitor) { return visitor->Visit(ast); }
    template<> void VisitAstVisitor(const Ides::ModuleDecl& ast, Ides::AstVisitor* visitor) { return visitor->Visit(ast); }
    template<> void VisitAstVisitor(const Ides::PartialFunction& ast, Ides::AstVisitor* visitor) { return visitor->Visit(ast); }
    template<> void VisitAstVisitor(const Ides::DataStructureDecl& ast, Ides::AstVisitor* visitor) { return visitor->Visit(ast); }

}

namespace Ides {
    void AstVisitor::DoVisit(const Ides::AstBase& ast) { DoVisitAstVisitor(ast, this); }
}
