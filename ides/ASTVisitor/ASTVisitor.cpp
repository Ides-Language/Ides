//
//  ASTVisitor.cpp
//  ides
//
//  Created by Sean Edwards on 12/30/12.
//
//

#include <ides/AST/AST.h>
#include <ides/AST/Expression.h>
#include <ides/AST/Statement.h>
#include <ides/AST/Declaration.h>

#include "ASTVisitor.h"


namespace Ides {
namespace AST {
    
    void Visitor::Visit(CompilationUnit* ast) {
        for (auto i = ast->begin(); i != ast->end(); ++i) {
            i->second->Accept(this);
        }
    }
    
}
}