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

    class AstVisitor;

    void DoVisitAstVisitor(const Ides::AstBase& ast, AstVisitor* visitor);

    class AstVisitor {
    public:
        virtual ~AstVisitor() { }

        virtual void DoVisit(const Ides::AstBase& ast) { DoVisitAstVisitor(ast, this); }

        virtual void Visit(const IdentifierExpr& ast) { }
        virtual void Visit(const ExprList& ast) { }
        virtual void Visit(const CallExpr& ast) { }
        virtual void Visit(const IndexExpr& ast) { }
        virtual void Visit(const ConstantExpr& ast) { }
        virtual void Visit(const BinaryExpr& ast) { }
        virtual void Visit(const UnaryExpr& ast) { }
        virtual void Visit(const Name& ast) { }
        virtual void Visit(const TraitDecl& ast) { }
        virtual void Visit(const ClassDecl& ast) { }
        virtual void Visit(const StructDecl& ast) { }
        virtual void Visit(const ValueDecl& ast) { }
        virtual void Visit(const ValDecl& ast) { }
        virtual void Visit(const VarDecl& ast) { }
        virtual void Visit(const FnDataDecl& ast) { }
        virtual void Visit(const FnDecl& ast) { }
        virtual void Visit(const ArgDecl& ast) { }
        virtual void Visit(const ModuleDecl& ast) { }
        virtual void Visit(const PartialFunction& ast) { }
        virtual void Visit(const DataStructureDecl& ast) { }
    };
}

#endif /* defined(__ides__AstVisitor__) */
