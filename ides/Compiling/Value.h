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
    
    class Value : public SymbolTable<Ides::Value*> {
    public:
        virtual ~Value() { }
    };
}

#endif /* defined(__ides__Value__) */
