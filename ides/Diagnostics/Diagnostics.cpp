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
        CreateDiagnostic(diags, COMPILER_NOT_IMPLEMENTED, clang::DiagnosticsEngine::Fatal, "feature not yet implemented");
        CreateDiagnostic(diags, IMPOSSIBLE_ERROR, clang::DiagnosticsEngine::Fatal, "logic error: %0");
        CreateDiagnostic(diags, BUILD_FAILED_ERRORS, clang::DiagnosticsEngine::Fatal, "build failed due to errors");
        
        CreateDiagnostic(diags, NOTE_FROM, clang::DiagnosticsEngine::Note, "from here");
        
        CreateDiagnostic(diags, PARSE_GENERIC_ERROR, clang::DiagnosticsEngine::Error, "parsing failed: %0");
        
        CreateDiagnostic(diags, FUNCTION_NO_RETURN, clang::DiagnosticsEngine::Error, "function does not return a value");
        CreateDiagnostic(diags, NO_IMPLICIT_CONVERSION, clang::DiagnosticsEngine::Error, "no implicit conversion from %0 to %1");
        CreateDiagnostic(diags, NO_EXPLICIT_CAST, clang::DiagnosticsEngine::Error, "cannot cast from %0 to incompatible type %1");
        
        CreateDiagnostic(diags, RETURN_FROM_VOID, clang::DiagnosticsEngine::Error, "returning an expression from a function with void return type");
        CreateDiagnostic(diags, RETURN_FROM_UNIT, clang::DiagnosticsEngine::Error, "functions with unit return type cannot return");
        CreateDiagnostic(diags, RETURN_NO_EXPRESSION, clang::DiagnosticsEngine::Error, "expected expression");
        
        CreateDiagnostic(diags, UNKNOWN_IDENTIFIER, clang::DiagnosticsEngine::Error, "unrecognized identifier %0");
        CreateDiagnostic(diags, UNKNOWN_MEMBER, clang::DiagnosticsEngine::Error, "%0 has no member named %1");
        CreateDiagnostic(diags, UNKNOWN_TYPE, clang::DiagnosticsEngine::Error, "no type %0 exists");
        
        CreateDiagnostic(diags, CALL_NON_FUNCTION, clang::DiagnosticsEngine::Error, "cannot call expression of type %0");
        CreateDiagnostic(diags, CALL_TOO_MANY_ARGS, clang::DiagnosticsEngine::Error, "too many arguments to function: expected %0, got %1");
        CreateDiagnostic(diags, CALL_INSUFFICIENT_ARGS, clang::DiagnosticsEngine::Error, "not enough arguments to function: expected %0, got %1");
        
        CreateDiagnostic(diags, OP_NO_SUCH_UNARY_OPERATOR, clang::DiagnosticsEngine::Error, "no such operator %0 on type %1");
        CreateDiagnostic(diags, OP_NO_SUCH_BINARY_OPERATOR, clang::DiagnosticsEngine::Error, "no such operator %0 on types %1, %2");
        
        CreateDiagnostic(diags, INVALID_TEMPORARY_VALUE, clang::DiagnosticsEngine::Error, "invalid use of temporary value");
        
        CreateDiagnostic(diags, BLOCK_UNREACHABLE_CODE, clang::DiagnosticsEngine::Warning, "unreachable code");
        
        CreateDiagnostic(diags, NOTE_DECLARED_HERE, clang::DiagnosticsEngine::Note, "originally declared here");
    }
    
    long CustomDiagToClangID(DiagIDs id) {
        return ids[id];
    }
    
}
}