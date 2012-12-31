//
//  ParseContext.h
//  ides
//
//  Created by Sean Edwards on 12/30/12.
//
//

#ifndef __ides__ParseContext__
#define __ides__ParseContext__

#include <ides/common.h>

#include <llvm/Support/MemoryBuffer.h>

#include <clang/Basic/SourceManager.h>
#include <clang/Basic/FileManager.h>
#include <clang/Basic/SourceLocation.h>

namespace Ides {
namespace AST {
    class AST;
}
    
namespace Parsing {
    
   
    class ParseContext {
    public:
        ParseContext(clang::SourceManager& srcMgr, const clang::FileEntry* fid);
        ~ParseContext();
        
        Ides::AST::AST* Parse();
        
        int ReadInput(char *buffer, int* numBytesRead, size_t maxBytesToRead);
        clang::SourceLocation GetFileStartLocation();
        
        void* GetScanner() const { return scanner; }
        
    protected:
        void InitParser();
        void DestroyParser();
        
    private:
        void* scanner;
        clang::SourceManager& srcMgr;
        const clang::FileEntry* file;
        clang::FileID fid;
        
        const llvm::MemoryBuffer* buf;
        size_t ptr;
    };
    
}
}

#endif /* defined(__ides__ParseContext__) */
