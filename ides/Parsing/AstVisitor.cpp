//
//  AstVisitor.cpp
//  ides
//
//  Created by Sean Edwards on 1/8/14.
//
//

#include "AstVisitor.h"

namespace Ides {
    DECL_AST_VISITOR(VisitAstVisitor, void, AstVisitor*);
    template<> void VisitAstVisitor(const IdentifierExpr& ast, AstVisitor* visitor) { return visitor->Visit(ast); }
    template<> void VisitAstVisitor(const ExprList& ast, AstVisitor* visitor) { return visitor->Visit(ast); }
    template<> void VisitAstVisitor(const CallExpr& ast, AstVisitor* visitor) { return visitor->Visit(ast); }
    template<> void VisitAstVisitor(const IndexExpr& ast, AstVisitor* visitor) { return visitor->Visit(ast); }
    template<> void VisitAstVisitor(const ConstantExpr& ast, AstVisitor* visitor) { return visitor->Visit(ast); }
    template<> void VisitAstVisitor(const BinaryExpr& ast, AstVisitor* visitor) { return visitor->Visit(ast); }
    template<> void VisitAstVisitor(const UnaryExpr& ast, AstVisitor* visitor) { return visitor->Visit(ast); }
    template<> void VisitAstVisitor(const Name& ast, AstVisitor* visitor) { return visitor->Visit(ast); }
    template<> void VisitAstVisitor(const TraitDecl& ast, AstVisitor* visitor) { return visitor->Visit(ast); }
    template<> void VisitAstVisitor(const ClassDecl& ast, AstVisitor* visitor) { return visitor->Visit(ast); }
    template<> void VisitAstVisitor(const StructDecl& ast, AstVisitor* visitor) { return visitor->Visit(ast); }
    template<> void VisitAstVisitor(const ValueDecl& ast, AstVisitor* visitor) { return visitor->Visit(ast); }
    template<> void VisitAstVisitor(const ValDecl& ast, AstVisitor* visitor) { return visitor->Visit(ast); }
    template<> void VisitAstVisitor(const VarDecl& ast, AstVisitor* visitor) { return visitor->Visit(ast); }
    template<> void VisitAstVisitor(const FnDataDecl& ast, AstVisitor* visitor) { return visitor->Visit(ast); }
    template<> void VisitAstVisitor(const FnDecl& ast, AstVisitor* visitor) { return visitor->Visit(ast); }
    template<> void VisitAstVisitor(const ArgDecl& ast, AstVisitor* visitor) { return visitor->Visit(ast); }
    template<> void VisitAstVisitor(const ModuleDecl& ast, AstVisitor* visitor) { return visitor->Visit(ast); }
    template<> void VisitAstVisitor(const PartialFunction& ast, AstVisitor* visitor) { return visitor->Visit(ast); }
    template<> void VisitAstVisitor(const DataStructureDecl& ast, AstVisitor* visitor) { return visitor->Visit(ast); }
}
