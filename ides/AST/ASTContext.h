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
#include <stack>

namespace Ides {
namespace AST {
    
    class DeclarationContext;
   
    class ASTContext {
    public:
        ASTContext(clang::IntrusiveRefCntPtr<clang::DiagnosticsEngine> diag) : diag(diag) { }
        
        llvm::LLVMContext& GetContext() { return llvm::getGlobalContext(); }
        
        clang::DiagnosticsEngine& GetDiagnostics() const { return *diag; }
        
        DeclarationContext* GetCurrentScope() {
            if (scopes.empty()) return NULL;
            return scopes.top();
        }
        std::stack<DeclarationContext*>& GetScopeStack() { return scopes; }
        
        class DeclScope {
        public:
            DeclScope(ASTContext& ctx, DeclarationContext* dctx);
            
            ~DeclScope() throw();
        private:
            ASTContext& ctx;
        };
    private:
        clang::IntrusiveRefCntPtr<clang::DiagnosticsEngine> diag;
        std::stack<DeclarationContext*> scopes;
    };
    
}
}

#endif /* defined(__ides__ASTContext__) */
