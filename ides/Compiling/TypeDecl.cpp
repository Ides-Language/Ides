//
//  TypeDecl.cpp
//  ides
//
//  Created by Sean Edwards on 1/5/14.
//
//

#include "TypeDecl.h"

namespace Ides {

    TypeDecl* RecordInitializer::Accept(const RecordDecl& ast) {
        Module* parent = this->IsEmpty<Module*>() ? NULL : this->Top<Module*>();
        RecordTypeDecl* trait = new RecordTypeDecl(ast, parent);
        for (auto& a : *ast.decl->body) {
            TypeDecl* decl = DoAccept(*a);
            if (decl) {
                trait->InsertSymbol(decl->GetName(), decl);
            }
        }
        return trait;
    }

    TypeDecl* RecordInitializer::Accept(const ModuleDecl& ast) {
        Module* parent = this->IsEmpty<Module*>() ? NULL : this->Top<Module*>();
        Module* mod = new Module(ast, parent);
        SETCTX(mod);
        for (auto& a : *ast.decl) {
            TypeDecl* decl = DoAccept(*a);
            if (decl) {
                mod->InsertSymbol(decl->GetName(), decl);
            }
        }
        return mod;
    }
}