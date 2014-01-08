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

        virtual void DoVisit(const Ides::AstBase& ast);

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

    template<typename T>
    class ReturningAstVisitor : private AstVisitor {
    public:
        virtual ~AstVisitor() { }

        virtual T DoAccept(const Ides::AstBase& ast) { DoVisit(ast, this); return last; }

        virtual T Accept(const IdentifierExpr& ast) { }
        virtual T Accept(const ExprList& ast) { }
        virtual T Accept(const CallExpr& ast) { }
        virtual T Accept(const IndexExpr& ast) { }
        virtual T Accept(const ConstantExpr& ast) { }
        virtual T Accept(const BinaryExpr& ast) { }
        virtual T Accept(const UnaryExpr& ast) { }
        virtual T Accept(const Name& ast) { }
        virtual T Accept(const TraitDecl& ast) { }
        virtual T Accept(const ClassDecl& ast) { }
        virtual T Accept(const StructDecl& ast) { }
        virtual T Accept(const ValueDecl& ast) { }
        virtual T Accept(const ValDecl& ast) { }
        virtual T Accept(const VarDecl& ast) { }
        virtual T Accept(const FnDataDecl& ast) { }
        virtual T Accept(const FnDecl& ast) { }
        virtual T Accept(const ArgDecl& ast) { }
        virtual T Accept(const ModuleDecl& ast) { }
        virtual T Accept(const PartialFunction& ast) { }
        virtual T Accept(const DataStructureDecl& ast) { }

    private:

        virtual void Visit(const IdentifierExpr& ast) { last = Accept(ast); }
        virtual void Visit(const ExprList& ast) { last = Accept(ast); }
        virtual void Visit(const CallExpr& ast) { last = Accept(ast); }
        virtual void Visit(const IndexExpr& ast) { last = Accept(ast); }
        virtual void Visit(const ConstantExpr& ast) { last = Accept(ast); }
        virtual void Visit(const BinaryExpr& ast) { last = Accept(ast); }
        virtual void Visit(const UnaryExpr& ast) { last = Accept(ast); }
        virtual void Visit(const Name& ast) { last = Accept(ast); }
        virtual void Visit(const TraitDecl& ast) { last = Accept(ast); }
        virtual void Visit(const ClassDecl& ast) { last = Accept(ast); }
        virtual void Visit(const StructDecl& ast) { last = Accept(ast); }
        virtual void Visit(const ValueDecl& ast) { last = Accept(ast); }
        virtual void Visit(const ValDecl& ast) { last = Accept(ast); }
        virtual void Visit(const VarDecl& ast) { last = Accept(ast); }
        virtual void Visit(const FnDataDecl& ast) { last = Accept(ast); }
        virtual void Visit(const FnDecl& ast) { last = Accept(ast); }
        virtual void Visit(const ArgDecl& ast) { last = Accept(ast); }
        virtual void Visit(const ModuleDecl& ast) { last = Accept(ast); }
        virtual void Visit(const PartialFunction& ast) { last = Accept(ast); }
        virtual void Visit(const DataStructureDecl& ast) { last = Accept(ast); }

        T last;
    };
}

#endif /* defined(__ides__AstVisitor__) */
