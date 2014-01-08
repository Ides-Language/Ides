//
//  TypeDecl.h
//  ides
//
//  Created by Sean Edwards on 1/5/14.
//
//

#ifndef __ides__TypeDecl__
#define __ides__TypeDecl__

#include <ides/common.h>
#include <ides/Compiling/Value.h>

namespace Ides {
    class TypeDecl : public Value {
    public:
        virtual Ides::StringRef GetName() = 0;
    };

    class TraitTypeDecl : public TypeDecl {
    public:
        TraitTypeDecl(const Ides::TraitDecl& ast) : ast(ast) {
            InsertSymbol(GetName(), this);
        }

        Ides::StringRef GetName() { return ast.name->ident->ident; }

        const Ides::TraitDecl& ast;
    };

    class ClassTypeDecl : public TypeDecl {
    public:
        ClassTypeDecl(const Ides::ClassDecl& ast) : ast(ast) {
            InsertSymbol(GetName(), this);
        }

        Ides::StringRef GetName() { return ast.name->ident->ident; }

        const Ides::ClassDecl& ast;
    };

    class StructTypeDecl : public TypeDecl {
    public:
        StructTypeDecl(const Ides::StructDecl& ast) : ast(ast) {
            InsertSymbol(GetName(), this);
        }

        Ides::StringRef GetName() { return ast.name->ident->ident; }

        const Ides::StructDecl& ast;
    };

    class Module : public Ides::TypeDecl {
    public:
        Module(const Ides::ModuleDecl& ast) : ast(ast) {
            InsertSymbol(GetName(), this);
        }

        Ides::StringRef GetName() { return ast.name->ident->ident; }

        const Ides::ModuleDecl& ast;
    };

}

#endif /* defined(__ides__TypeDecl__) */
