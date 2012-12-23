//
//  ASTStatement.h
//  ides
//
//  Created by Sean Edwards on 12/14/12.
//
//

#ifndef ides_ASTStatement_h
#define ides_ASTStatement_h

#include <ides/AST/AST.h>

namespace Ides {
namespace AST {

    class ASTExpression;
    
    class ASTStatement : public AST {
        
    };
    
    class ASTCompoundStatement : public ASTList {
    public:
        ASTCompoundStatement() { }
        virtual ~ASTCompoundStatement() { }
        
        virtual llvm::Value* GetValue(ParseContext& ctx);
    };
    
    class ASTIfStatement : public ASTStatement {
    public:
        ASTIfStatement(ASTExpression* expr, ASTStatement* ift, ASTStatement* iff) : condition(expr), iftrue(ift), iffalse(iff) { }
        virtual ~ASTIfStatement() {
            delete condition;
            delete iftrue;
            if (iffalse) delete iffalse;
        }
        
        virtual llvm::Value* GetValue(ParseContext& ctx);
        
        ASTExpression* condition;
        ASTStatement* iftrue;
        ASTStatement* iffalse;
    };
    
    class ASTWhileStatement : public ASTStatement {
    public:
        ASTWhileStatement(ASTExpression* expr, ASTStatement* body) : condition(expr), body(body) { }
        virtual ~ASTWhileStatement() {
            delete condition;
            delete body;
        }
        
        virtual llvm::Value* GetValue(ParseContext& ctx);
        
        ASTExpression* condition;
        ASTStatement* body;
    };
    
    
}
}

#endif
