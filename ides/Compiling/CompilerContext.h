//
//  CompilerContext.h
//  ides
//
//  Created by Sean Edwards on 1/8/14.
//
//

#ifndef __ides__CompilerContext__
#define __ides__CompilerContext__

#include <ides/common.h>
#include <ides/Compiling/TypeDecl.h>

namespace Ides {
    
struct CompilerContext : MultiContext<Ides::TypeDecl*> {

};

}

#endif /* defined(__ides__CompilerContext__) */
