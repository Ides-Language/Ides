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
    
    const Ides::Types::Type* FunctionCallExpression::GetType(ASTContext& ctx) const {
        const Ides::Types::Type* ft = fn->GetType(ctx);
        const Ides::Types::FunctionType* fntype = dynamic_cast<const Ides::Types::FunctionType*>(ft);
        if (fn) return fntype->retType;
        Ides::Diagnostics::Diag(ctx.GetDiagnostics(), Ides::Diagnostics::CALL_NON_FUNCTION, this->exprloc.getBegin()) << ft->ToString();
        return NULL;
    }
    
}
}