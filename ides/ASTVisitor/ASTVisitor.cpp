//
//  ASTVisitor.cpp
//  ides
//
//  Created by Sean Edwards on 12/30/12.
//
//

#include "ASTVisitor.h"

#include <ides/AST/Declaration.h>

namespace Ides {
namespace AST {
    
    void Visitor::Visit(CompilationUnit* ast) {
        for (auto i = ast->begin(); i != ast->end(); ++i) {
            (*i)->Accept(this);
        }
    }
    
}
}