//
//  DeclarationContext.cpp
//  ides
//
//  Created by Sean Edwards on 12/30/12.
//
//

#include "DeclarationContext.h"
#include "Declaration.h"

namespace Ides {
namespace AST {
    
    Declaration* ConcreteDeclarationContext::GetMember(ASTContext& ctx, Ides::StringRef name) const {
        auto i = this->members.find(name);
        if (i != this->members.end()) return i->second;
        return NULL;
    }
    
    void ConcreteDeclarationContext::AddMember(Ides::StringRef name, Ides::AST::Declaration* decl) {
        HierarchicalConcreteDeclarationContext* hdecl = dynamic_cast<HierarchicalConcreteDeclarationContext*>(decl);
        if (hdecl) {
            hdecl->SetParent(this);
        }
        auto i = this->members.find(name);
        if (i != this->members.end()) {
            if (dynamic_cast<FunctionDeclaration*>(i->second)) {
                OverloadedFunction* of = new OverloadedFunction();
                of->push_back(i->second);
                this->members[i->first] = of;
                return;
            }
        }
        members.insert(std::make_pair(name, decl));
    }
    
    Declaration* HierarchicalConcreteDeclarationContext::GetMember(ASTContext& ctx, Ides::StringRef name) const {
        Ides::AST::Declaration* decl = ConcreteDeclarationContext::GetMember(ctx, name);
        if (decl) return decl;
        else if (this->parentContext) return this->parentContext->GetMember(ctx, name);
        return NULL;
    }
}
}