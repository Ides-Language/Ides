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
        std::vector<llvm::Value*> args;

        args.push_back(llvm::MDString::get(lctx, ast->GetName()));

        last = llvm::MDNode::get(lctx, args);
    }
    
    llvm::MDNode* MetadataSerializer::GetMDValue(AST* ast) {
        last = NULL;
        auto i = mdnodes.find(ast);
        if (i != mdnodes.end()) return i->second;
        
        ast->Accept(this);
        mdnodes.insert(std::make_pair(ast, last));
        return last;
    }
    
}
}