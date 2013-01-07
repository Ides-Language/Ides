//
//  Project.cpp
//  ides
//
//  Created by Sean Edwards on 12/30/12.
//
//

#include "Project.h"

#include <boost/filesystem.hpp>

#include <ides/Parsing/ParseContext.h>

namespace Ides {
namespace Project {
   
    Project::Project(llvm::IntrusiveRefCntPtr<clang::DiagnosticsEngine> diag, Ides::AST::ASTContext& actx) : actx(actx), diag(diag) {
        InitManagers();
    }
    
    Project::Project(llvm::IntrusiveRefCntPtr<clang::DiagnosticsEngine> diag, Ides::AST::ASTContext& actx, llvm::StringRef projfile) : actx(actx), diag(diag) {
        InitManagers();
    }
    
    void Project::InitManagers() {
        clang::FileSystemOptions fsopts;
        this->fman = new clang::FileManager(fsopts);
        
        this->sman = new clang::SourceManager(*this->diag, *this->fman);
    }
    
    Project::~Project() {
        
    }
    
    Ides::AST::AST* Project::ParseFile(llvm::StringRef srcfile) {
        Ides::Parsing::ParseContext ctx(diag, *sman, fman->getFile(srcfile));
        return ctx.Parse();
    }
    
}
}