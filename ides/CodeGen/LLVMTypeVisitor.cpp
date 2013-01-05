//
//  LLVMTypeVisitor.cpp
//  ides
//
//  Created by Sean Edwards on 1/4/13.
//
//

#include "LLVMTypeVisitor.h"

namespace Ides {
namespace CodeGen {
    
    void LLVMTypeVisitor::Visit(const Ides::Types::FunctionType *ty) {
        std::vector<llvm::Type*> llvmargTypes;
        for (auto i = ty->argTypes.begin(); i != ty->argTypes.end(); ++i) {
            const Ides::Types::FunctionType* argasfunction = dynamic_cast<const Ides::Types::FunctionType*>(*i);
            if (argasfunction) {
                Ides::Types::PointerType::Get(*i)->Accept(this);
                llvmargTypes.push_back(this->t);
            } else {
                (*i)->Accept(this);
                llvmargTypes.push_back(this->t);
            }
        }
        const Ides::Types::FunctionType* retasfunction = dynamic_cast<const Ides::Types::FunctionType*>(ty->retType);
        llvm::FunctionType *FT = NULL;
        if (retasfunction) {
            Ides::Types::PointerType::Get(ty->retType)->Accept(this);
            FT = llvm::FunctionType::get(this->t,llvmargTypes, false);
        }
        else {
            ty->retType->Accept(this);
            FT = llvm::FunctionType::get(this->t,llvmargTypes, false);
        }
        t = FT;
        
    }
    
    void LLVMTypeVisitor::Visit(const Ides::Types::StructType *ty) {
        auto i = this->structTypes.find(ty->GetName());
        if (i != this->structTypes.end()) {
            t = i->second;
            return;
        }
        llvm::StructType* st = llvm::StructType::create(ctx, ty->GetName());
        this->structTypes.insert(std::make_pair(ty->GetName(), st));
        
        t = st;
    }
    
}
}