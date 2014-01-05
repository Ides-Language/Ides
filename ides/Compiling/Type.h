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

namespace Ides {
    template<typename T>
    struct Ty {
        typedef std::vector<const T*> Many;
        typedef const T* One;
    };

    struct Type : Ty<Type> {
    };

    struct TraitTy : Type, Ty<TraitTy> {

    };

    struct StructTy : Type, Ty<StructTy> {

    };

    struct ClassTy : StructTy, Ty<ClassTy> {

    };

    struct BuiltinTy : Type, Ty<BuiltinTy> {

    };

    struct UnitTy : BuiltinTy, Ty<UnitTy>, public Ides::Util::Singleton<UnitTy> {

    };

    struct VoidTy : BuiltinTy, Ty<VoidTy>, public Ides::Util::Singleton<VoidTy> {

    };
}

#endif /* defined(__ides__Type__) */
