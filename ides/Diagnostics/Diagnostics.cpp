//
//  Diagnostics.cpp
//  ides
//
//  Created by Sean Edwards on 12/30/12.
//
//

#include "Diagnostics.h"

namespace {
    static unsigned ids[Ides::Diagnostics::DIAGS_COUNT];
    
    void CreateDiagnostic(clang::DiagnosticsEngine& diags, Ides::Diagnostics::DiagIDs id, clang::DiagnosticsEngine::Level lvl, const char* msg) {
        ids[id] = diags.getCustomDiagID(lvl, msg);
    }
}


namespace Ides {
namespace Diagnostics {
    
    
    void InitAllDiagnostics(clang::DiagnosticsEngine& diags) {
        CreateDiagnostic(diags, FUNCTION_NO_RETURN, clang::DiagnosticsEngine::Error, "function does not return a value");
        CreateDiagnostic(diags, NO_IMPLICIT_CONVERSION, clang::DiagnosticsEngine::Error, "no implicit conversion from %0 to %1");
        
        CreateDiagnostic(diags, RETURN_FROM_VOID, clang::DiagnosticsEngine::Error, "returning an expression from a function with void return type");
        CreateDiagnostic(diags, RETURN_FROM_UNIT, clang::DiagnosticsEngine::Error, "functions with unit return type cannot return");
        CreateDiagnostic(diags, RETURN_NO_EXPRESSION, clang::DiagnosticsEngine::Error, "expected expression");
        
        CreateDiagnostic(diags, BLOCK_UNREACHABLE_CODE, clang::DiagnosticsEngine::Warning, "unreachable code");
    }
    
    clang::DiagnosticBuilder Diag(clang::DiagnosticsEngine& diags, Ides::Diagnostics::DiagIDs id) {
        return diags.Report(ids[id]);
    }
    
    clang::DiagnosticBuilder Diag(clang::DiagnosticsEngine& diags, Ides::Diagnostics::DiagIDs id, clang::SourceLocation loc) {
        return diags.Report(loc, ids[id]);
    }
    
}
}