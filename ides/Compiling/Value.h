//
//  Value.h
//  ides
//
//  Created by Sean Edwards on 1/7/14.
//
//

#ifndef __ides__Value__
#define __ides__Value__

#include <ides/common.h>
#include <ides/Parsing/AST.h>
#include <ides/Compiling/SymbolTable.h>

namespace Ides {
    class TypeDecl;
    class Type;
    
    class Value {
    public:
        virtual ~Value() { }
        /*virtual Ides::Value*& GetSymbol(const llvm::StringRef str, Ides::Visibility vis = V_PUBLIC) {
            auto pair = str.split('.');
            Ides::Value*& lhs = VisibilitySymbolTable<Ides::Value*>::GetSymbol(pair.first, vis);
            if (pair.second.empty()) return lhs;
            return lhs->GetSymbol(pair.second, vis);
        }*/
    };

    template<typename T>
    class ValueImpl : public Value {
    };
}

#endif /* defined(__ides__Value__) */
