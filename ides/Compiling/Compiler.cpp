//
//  Compiler.cpp
//  ides
//
//  Created by Sean Edwards on 1/3/14.
//
//

#include "Compiler.h"

void Ides::Compiler::Compile(Ides::SourcePackage& package) {
    DBG("Compiling " << package.GetProperty<Ides::String>("name") << "...");
}
