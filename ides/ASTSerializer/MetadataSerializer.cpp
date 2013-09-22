//
//  MetadataSerializer.cpp
//  ides
//
//  Created by Sean Edwards on 1/8/13.
//
//

#include "MetadataSerializer.h"

namespace Ides {
namespace AST {
    
    
    void MetadataSerializer::Visit(Ides::AST::FunctionDeclaration* ast) {
        last = node()
        .Add("function")
        .Add(ast->GetName())
        .Add(ast->GetType(actx))
        ;
    }


    void MetadataSerializer::Visit(Ides::AST::OverloadedFunction* ast) {
        last = node()
        .Add("overloaded function")
        .Add(*ast)
        ;
    }


    void MetadataSerializer::Visit(Ides::AST::StructDeclaration* ast) {
        last = node()
        .Add("struct")
        .Add(ast->GetName())
        .Add(ast->GetType(actx))
        ;
    }

    void MetadataSerializer::Visit(const Ides::Types::FunctionType* ty) {
        last = node()
        .Add("fn")
        .Add(ty->GetReturnType())
        .Add(ty->GetArgTypes())
        ;
    }



    void MetadataSerializer::Visit(const Ides::Types::StructType* ty) {
        last = node()
        .Add("struct")
        .Add(ty->GetMembers())
        ;
    }


    llvm::MDNode* MetadataSerializer::GetMDValue(AST* ast) {
        last = NULL;
        auto i = mdnodes.find(ast);
        if (i != mdnodes.end()) return i->second;

        ast->Accept(this);
        mdnodes.insert(std::make_pair(ast, last));
        return last;
    }
    
    llvm::MDNode* MetadataSerializer::GetMDValue(const Ides::Types::Type* type) {
        last = NULL;
        auto i = mdtypes.find(type);
        if (i != mdtypes.end()) return i->second;
        
        type->Accept(this);
        mdtypes.insert(std::make_pair(type, last));
        return last;
    }
    
}
}