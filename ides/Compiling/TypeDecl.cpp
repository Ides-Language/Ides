//
//  TypeDecl.cpp
//  ides
//
//  Created by Sean Edwards on 1/5/14.
//
//

#include "TypeDecl.h"

namespace Ides {

    /*
    template<> TypeDecl* GenerateTypeDecl(const TraitDecl& ast, CompilerContext* compiler) {
        TraitTypeDecl* trait = new TraitTypeDecl(ast);
        return trait;
    }

    template<> TypeDecl* GenerateTypeDecl(const ClassDecl& ast, CompilerContext* compiler) {
        ClassTypeDecl* trait = new ClassTypeDecl(ast);
        return trait;
    }

    template<> TypeDecl* GenerateTypeDecl(const StructDecl& ast, CompilerContext* compiler) {
        StructTypeDecl* trait = new StructTypeDecl(ast);
        return trait;
    }

    template<> TypeDecl* GenerateTypeDecl(const FnDecl& ast, CompilerContext* compiler) { return NULL; }

    template<> TypeDecl* GenerateTypeDecl(const ModuleDecl& ast, CompilerContext* compiler) {
        Module* mod = new Module(ast);
        for (auto& a : *ast.decl) {
            TypeDecl* decl = DoGenerateTypeDecl(*a, compiler);
            if (decl) {
                mod->InsertSymbol(decl->GetName(), decl);
            } else {
                MSG(E_MOD_NONDECL) % a->getName();
            }
        }
        return mod;
    }
     */
}