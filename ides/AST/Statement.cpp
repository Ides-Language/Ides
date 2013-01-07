//
//  Statement.cpp
//  ides
//
//  Created by Sean Edwards on 12/30/12.
//
//

#include "Statement.h"
#include <ides/ASTVisitor/ASTVisitor.h>

namespace Ides {
namespace AST {
    
    void Block::Accept(Visitor* v) { v->Visit(this); }
    void IfStatement::Accept(Visitor* v) { v->Visit(this); }
    void WhileStatement::Accept(Visitor* v) { v->Visit(this); }
    void ForStatement::Accept(Visitor* v) { v->Visit(this); }
}
}