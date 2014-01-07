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
#include <ides/Compiling/SemGraph.h>
#include <ides/Util/MultiStack.h>

namespace Ides {

    struct A { };
    struct B { };

    struct Type : public Ides::MultiContext<A*, B*> {
    public:
        Type() {
            A* x = new A();
            SETCTX(x);
            delete x;
        }
    };

}

#endif /* defined(__ides__Type__) */
