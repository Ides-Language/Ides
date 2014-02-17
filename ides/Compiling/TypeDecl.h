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
#include <ides/Parsing/AstVisitor.h>
#include <ides/Compiling/Value.h>

namespace Ides {
    class TypeDecl : public VisibilitySymbolTable<Ides::TypeDecl*> {
    public:
        virtual Ides::String GetFullName() const { return GetName(); }
        virtual Ides::String GetName() const = 0;
        //virtual Ides::Visibility GetVis() const = 0;
    };

    class Module : public Ides::TypeDecl {
    public:
        Module(const Ides::ModuleDecl& ast, Module* parent) : mySuper(parent), ast(ast) {
            InsertSymbol(GetName(), this);
            DBG("Created module " << GetFullName());
        }

        //Ides::Visibility GetVis() const { return ast.vis; }
        Ides::String GetName() const { return ast.name->ident->ident; }
        Ides::String GetFullName() const {
            if (mySuper == NULL) return GetName();
            return StringBuilder() << mySuper->GetFullName() + "." + this->GetName();
        }

        bool operator==(llvm::StringRef name) const {
            if (mySuper != NULL) {
                auto pair = name.rsplit('.');
                return GetName() == pair.second && *mySuper == pair.first;
            }
            return name == GetName();
        }

        const Module* mySuper;
        const Ides::ModuleDecl& ast;
    };

    class RecordTypeDecl : public TypeDecl {
    public:
        RecordTypeDecl(const Ides::RecordDecl& ast, Module* parent) : mySuper(parent), ast(ast) {
            InsertSymbol(GetName(), this);
            DBG("Created record " << GetFullName());
        }

        //Ides::Visibility GetVis() const { return ast.vis; }
        Ides::String GetName() const { return ast.name->ident->ident; }
        Ides::String GetFullName() const {
            if (mySuper == NULL) return GetName();
            return StringBuilder() << mySuper->GetFullName() + "." + this->GetName();
        }

        const Module* mySuper;
        const Ides::RecordDecl& ast;
    };

    class RecordInitializer
        : public ReturningAstVisitor<TypeDecl*>,
          public MultiContext<Module*> {
    public:

        virtual TypeDecl* Accept(const RecordDecl& ast);
        virtual TypeDecl* Accept(const ModuleDecl& ast);
    };

}

#endif /* defined(__ides__TypeDecl__) */
