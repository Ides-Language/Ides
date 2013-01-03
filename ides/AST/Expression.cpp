//
//  Expression.cpp
//  ides
//
//  Created by Sean Edwards on 12/30/12.
//
//

#include "Expression.h"
#include "Declaration.h"

namespace Ides {
namespace Util {
    
}
    
namespace AST {
    
    const Ides::Types::Type* DotExpression::GetType(ASTContext& ctx) const {
        Expression* exp = lhs.get();
        Declaration* decl = exp->GetMember(ctx, **rhs);
        if (decl) return decl->GetType(ctx);
        Ides::Diagnostics::Diag(ctx.GetDiagnostics(), Ides::Diagnostics::UNKNOWN_MEMBER, this->exprloc.getBegin()) << lhs->GetType(ctx)->ToString() << **rhs;
        return NULL;
    }
    
    const Ides::Types::Type* IdentifierExpression::GetType(ASTContext& ctx) const {
        Declaration* decl = ctx.GetCurrentScope()->GetMember(ctx, **tok);
        if (decl) return decl->GetType(ctx);
        return NULL;
    }
    
}
}