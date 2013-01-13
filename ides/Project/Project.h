//
//  Project.h
//  ides
//
//  Created by Sean Edwards on 12/30/12.
//
//

#ifndef __ides__Project__
#define __ides__Project__

#include <ides/AST/AST.h>

namespace Ides {
namespace Project {
    
    class Project {
    public:
        Project(llvm::IntrusiveRefCntPtr<clang::DiagnosticsEngine> diag, Ides::AST::ASTContext& actx);
        Project(llvm::IntrusiveRefCntPtr<clang::DiagnosticsEngine> diag, Ides::AST::ASTContext& actx, llvm::StringRef projfile);
        ~Project();
        
        Ides::AST::AST* ParseFile(llvm::StringRef srcfile);
        
        llvm::Module* Compile(Ides::AST::CompilationUnit* cu);
        
    private:
        void InitManagers();
        
        clang::FileManager* fman;
        clang::SourceManager* sman;
        
        llvm::IntrusiveRefCntPtr<clang::DiagnosticsEngine> diag;
        
        Ides::AST::ASTContext& actx;
    };
    
    
}
}

#endif /* defined(__ides__Project__) */
