//
//  ASTContext.cpp
//  ides
//
//  Created by Sean Edwards on 12/30/12.
//
//

#include "ASTContext.h"
#include <ides/AST/DeclarationContext.h>

namespace Ides {
namespace AST {
    
    ASTContext::DeclScope::DeclScope(ASTContext& ctx, DeclarationContext* dctx) : ctx(ctx) {
        HierarchicalConcreteDeclarationContext* hdecl = dynamic_cast<HierarchicalConcreteDeclarationContext*>(dctx);
        if (hdecl) {
            hdecl->SetParent(ctx.GetCurrentScope());
            LOG("Hierarchical DeclScope Push");
        } else {
            LOG("DeclScope Push");
        }
        ctx.GetScopeStack().push(dctx);
    }
    
    ASTContext::DeclScope::~DeclScope() throw() {
        LOG("DeclScope Pop");
        ctx.GetScopeStack().pop();
    }
    
}
}