//
//  Statement.h
//  ides
//
//  Created by Sean Edwards on 12/30/12.
//
//

#ifndef __ides__Statement__
#define __ides__Statement__


#include <ides/AST/DeclarationContext.h>
#include <ides/AST/AST.h>

namespace Ides {
namespace AST {
    class Expression;
    class Statement : public AST {
        
    };

    class Block : public Statement, public HierarchicalConcreteDeclarationContext {
    public:
        virtual void Accept(Visitor* v);
        
        std::list<Statement*> statements;
    };
    
    class IfStatement : public Statement, public HierarchicalConcreteDeclarationContext {
    public:
        IfStatement(Expression* expr, Statement* ift, Statement* iff) : condition(expr), iftrue(ift), iffalse(iff) { }
        virtual void Accept(Visitor* v);
        
        Expression* condition;
        Statement* iftrue;
        Statement* iffalse;
    };
    
    class WhileStatement : public Statement, public HierarchicalConcreteDeclarationContext {
    public:
        WhileStatement(Expression* expr, Statement* body) : condition(expr), body(body) { }
        virtual void Accept(Visitor* v);
        
        Expression* condition;
        Statement* body;
    };
    
    class ForStatement : public Statement, public HierarchicalConcreteDeclarationContext {
    public:
        ForStatement(Statement* startexpr, Expression* endexpr, Expression* eachexpr, Statement* body) :
            startexpr(startexpr), endexpr(endexpr), eachexpr(eachexpr), body(body) { }
        virtual void Accept(Visitor* v);
        
        Statement* startexpr;
        Expression* endexpr;
        Expression* eachexpr;
        Statement* body;
    };
}
}

#endif /* defined(__ides__Statement__) */
