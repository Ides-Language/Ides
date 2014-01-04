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
#include <ides/Parsing/Parser.h>
#include <ides/Source/SourceLocation.h>
#include <ides/Source/SourcePackage.h>

namespace Ides {
    class Compiler : public Parser {
    public:
        void Compile(Ides::SourcePackage& package);
    };
}

#endif /* defined(__ides__Compiler__) */
