//
//  Diagnostics.h
//  ides
//
//  Created by Sean Edwards on 12/30/12.
//
//

#ifndef __ides__Diagnostics__
#define __ides__Diagnostics__

#include <ides/common.h>

namespace Ides {
namespace Diagnostics {
    enum DiagIDs {
        NO_IMPLICIT_CONVERSION,
        FUNCTION_NO_RETURN,
        
        RETURN_FROM_VOID,
        RETURN_FROM_UNIT,
        RETURN_NO_EXPRESSION,
        
        BLOCK_UNREACHABLE_CODE,
        
        DIAGS_COUNT
    };
    
    void InitAllDiagnostics(clang::DiagnosticsEngine& diags);
    
    clang::DiagnosticBuilder Diag(clang::DiagnosticsEngine& diags, Ides::Diagnostics::DiagIDs id);
    clang::DiagnosticBuilder Diag(clang::DiagnosticsEngine& diags, Ides::Diagnostics::DiagIDs id, clang::SourceLocation loc);
}
}

#endif /* defined(__ides__Diagnostics__) */
