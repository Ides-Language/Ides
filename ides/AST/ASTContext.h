//
//  ASTContext.h
//  ides
//
//  Created by Sean Edwards on 12/30/12.
//
//

#ifndef __ides__ASTContext__
#define __ides__ASTContext__

#include <ides/common.h>

namespace Ides {
namespace AST {
   
    class ASTContext {
    public:
        
        llvm::LLVMContext& GetContext() { return llvm::getGlobalContext(); }
        
        clang::DiagnosticsEngine& GetDiagnostics() const { return *diag; }
    private:
        clang::IntrusiveRefCntPtr<clang::DiagnosticsEngine> diag;
    };
    
}
}

#endif /* defined(__ides__ASTContext__) */
