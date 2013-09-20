//
//  DIGen.cpp
//  ides
//
//  Created by Sean Edwards on 1/12/13.
//
//

#include "DIGen.h"

namespace Ides {
namespace CodeGen {
    
    void DIGenerator::Visit(const Ides::Types::FunctionType* ty) {
        
    }
    
    void DIGenerator::Visit(const Ides::Types::StructType* ty) {
        std::vector<llvm::Value*> membersDI;
        for (auto i = ty->GetMembers().begin(); i != ty->GetMembers().end(); ++i) {
            membersDI.push_back(GetType(i->second));
        }
        
        createStructType(llvm::DIDescriptor(GetCurrentScope()),
                         ty->GetName(),
                         llvm::DIFile(),
                         0,
                         ty->GetSize(),
                         ty->GetAlignment(),
                         0,
                         llvm::DIType(),
                         getOrCreateArray(membersDI));
    }
    
}
}