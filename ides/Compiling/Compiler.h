//
//  Compiler.h
//  ides
//
//  Created by Sean Edwards on 1/3/14.
//
//

#ifndef __ides__Compiler__
#define __ides__Compiler__

#include <ides/common.h>
#include <ides/Source/SourceLocation.h>
#include <ides/Source/SourcePackage.h>

namespace Ides {
    struct AstBase;
    class Compiler {
    public:
        void Compile(const AstBase&);
    };
}

#endif /* defined(__ides__Compiler__) */
