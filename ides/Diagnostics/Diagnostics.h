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
        
        UNKNOWN_IDENTIFIER,
        UNKNOWN_MEMBER,
        
        BLOCK_UNREACHABLE_CODE,
        
        
        NOTE_DECLARED_HERE,
        
        DIAGS_COUNT
    };
    
    long CustomDiagToClangID(DiagIDs id);
    void InitAllDiagnostics(clang::DiagnosticsEngine& diags);
    
    
    inline clang::DiagnosticBuilder Diag(clang::DiagnosticsEngine& diags, Ides::Diagnostics::DiagIDs id) {
        return diags.Report(CustomDiagToClangID(id));
    }
    
    inline clang::DiagnosticBuilder Diag(clang::DiagnosticsEngine& diags, Ides::Diagnostics::DiagIDs id, clang::SourceLocation loc) {
        return diags.Report(loc, CustomDiagToClangID(id));
    }
}
}

#endif /* defined(__ides__Diagnostics__) */
