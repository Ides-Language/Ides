//
//  Compiler.cpp
//  ides
//
//  Created by Sean Edwards on 1/3/14.
//
//

#include "Compiler.h"
#include <ides/Compiling/CompilerContext.h>

#include <ides/Compiling/TypeDecl.h>

namespace Ides {
    void Compiler::Compile(const Ast& ast) {
        RecordInitializer recordinit;
        TypeDecl* mod = recordinit.DoAccept(ast);
        DBG("Accepted module: " << mod->GetFullName());
        delete mod;
        //DoGenerateTypeDecl(ast, &ctx);
    }
}