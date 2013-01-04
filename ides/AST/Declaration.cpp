//
//  Declaration.cpp
//  ides
//
//  Created by Sean Edwards on 12/30/12.
//
//

#include "Declaration.h"

namespace Ides {
namespace AST {
    
    const Ides::Types::Type* FunctionDeclaration::GetReturnType(ASTContext &ctx) {
        if (val) return val->GetType(ctx);
        else if (returntype) return returntype->GetType(ctx);
        return Ides::Types::VoidType::GetSingletonPtr();
    }
    
    const Ides::Types::Type* FunctionDeclaration::GetType(ASTContext& ctx)    {
        class RecursiveTypeEvalException {
        public:
            RecursiveTypeEvalException(clang::SourceRange loc) : loc(loc) {}
            virtual ~RecursiveTypeEvalException() throw() {}
            
            const clang::SourceRange loc;
        };
        
        // No need to re-calculate the type.
        if (functype != NULL) return functype;
        
        if (evaluatingtype) {
            throw RecursiveTypeEvalException(this->val->exprloc);
        }
        evaluatingtype = true;
        
        std::vector<const Ides::Types::Type*> argTypes;
        for (auto i = this->args.begin(); i != this->args.end(); ++i) {
            argTypes.push_back((*i)->GetType(ctx));
        }
        
        const Ides::Types::Type* ret = NULL;
        
        if (this->returntype == NULL) {
            if (val == NULL) ret = Ides::Types::VoidType::GetSingletonPtr();
            else {
                try {
                    ret = val->GetType(ctx);
                } catch (const RecursiveTypeEvalException& ex) {
                    this->evaluatingtype = false;
                    throw RecursiveTypeEvalException(this->val->exprloc);
                }
            }
        } else {
            ret = this->returntype->GetType(ctx);
        }
        
        evaluatingtype = false;
        assert(ret != NULL);
        this->functype = Ides::Types::FunctionType::Get(ret, argTypes);
        return this->functype;
    }
    
    void StructDeclaration::GenType(ASTContext& ctx) {
        Ides::Types::StructType* st = Ides::Types::StructType::GetOrCreate(ctx, this->GetName());
        
        std::vector<std::pair<Ides::String, const Ides::Types::Type*> > membertypes;
        for (auto i = this->members.begin(); i != this->members.end(); ++i) {
            NamedDeclaration* decl = (NamedDeclaration*)*i;
            membertypes.push_back(std::make_pair(decl->GetName(), decl->GetType(ctx)));
            st->AddMember(decl->GetName(), decl);
        }
        st->SetMembers(ctx, membertypes);
    }
}
}