//
//  Type.h
//  ides
//
//  Created by Sean Edwards on 1/3/14.
//
//

#ifndef __ides__Type__
#define __ides__Type__

#include <ides/common.h>
#include <ides/Compiling/Value.h>
#include <ides/Util/MultiStack.h>

namespace Ides {
    class Type  : public Value {
    public:
        static const char* AnonymousName;

        Type() {
        }
    };

}

#endif /* defined(__ides__Type__) */
