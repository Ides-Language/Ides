//
//  ParseContext.cpp
//  ides
//
//  Created by Sean Edwards on 12/30/12.
//
//

#include "ParseContext.h"
#include <ides/AST/AST.h>
#include <algorithm>

int yyparse (Ides::Parsing::ParseContext* context, Ides::AST::AST** program);

namespace Ides {
namespace Parsing {
   
    
    ParseContext::ParseContext(llvm::IntrusiveRefCntPtr<clang::DiagnosticsEngine> diag, clang::SourceManager& srcMgr, const clang::FileEntry* file)
        : diag(diag), srcMgr(srcMgr), file(file), ptr(0)
    {
        this->fid = srcMgr.createMainFileID(file);
        this->buf = srcMgr.getMemoryBufferForFile(file);
        this->InitParser();
    }
    
    ParseContext::~ParseContext() {
        this->DestroyParser();
    }
    
    Ides::AST::AST* ParseContext::Parse() {
        Ides::AST::AST* ret;
        yyparse(this, &ret);
        static_cast<Ides::AST::CompilationUnit*>(ret)->SetFile(this->fid);
        return ret;
    }
    
    int ParseContext::ReadInput(char *buffer, int* numBytesRead, size_t maxBytesToRead) {
        if (ptr == buf->getBufferSize()) return 0;
        
        size_t bytes = std::max(maxBytesToRead, buf->getBufferSize() - ptr);
        *numBytesRead = bytes;
        memcpy(buffer, buf->getBufferStart() + ptr, bytes);
        ptr += bytes;
        return 1;
    }
    
    clang::SourceLocation ParseContext::GetFileStartLocation() {
        return srcMgr.getLocForStartOfFile(this->fid);
    }
    
}
}