//
//  Diagnostics.h
//  ides
//
//  Created by Sean Edwards on 12/30/12.
//
//

#ifndef __ides__Diagnostics__
#define __ides__Diagnostics__

#include <clang/Basic/Diagnostic.h>
#include <clang/Basic/DiagnosticIDs.h>

namespace Ides {
namespace Diagnostics {
    enum DiagIDs {
        COMPILER_NOT_IMPLEMENTED,
        IMPOSSIBLE_ERROR,
        
        NOTE_FROM,
        
        PARSE_GENERIC_ERROR,
        
        NO_IMPLICIT_CONVERSION,
        NO_EXPLICIT_CAST,
        FUNCTION_NO_RETURN,
        
        RETURN_FROM_VOID,
        RETURN_FROM_UNIT,
        RETURN_NO_EXPRESSION,
        
        UNKNOWN_IDENTIFIER,
        UNKNOWN_MEMBER,
        
        CALL_NON_FUNCTION,
        CALL_TOO_MANY_ARGS,
        CALL_INSUFFICIENT_ARGS,
        
        OP_NO_SUCH_OPERATOR,
        
        BLOCK_UNREACHABLE_CODE,
        
        INVALID_TEMPORARY_VALUE,
        
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
