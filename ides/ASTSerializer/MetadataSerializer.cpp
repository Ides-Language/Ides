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
        last = llvm::MDString::get(lctx, "function");
    }
    
    llvm::Value* MetadataSerializer::GetMDValue(AST* ast) {
        auto i = mdnodes.find(ast);
        if (i != mdnodes.end()) return i->second;
        
        ast->Accept(this);
        mdnodes.insert(std::make_pair(ast, last));
        return last;
    }
    
}
}