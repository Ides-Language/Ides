#include <ides/AST/AST.h>
#include <ides/AST/ASTExpression.h>
#include <ides/AST/ASTConstantExpression.h>

#include <boost/uuid/uuid_generators.hpp> // generators
#include <boost/uuid/uuid_io.hpp>         // streaming operators etc.

#include <llvm/BasicBlock.h>
#include <llvm/Function.h>
#include "llvm/Analysis/Verifier.h"

#include <sstream>
#include <queue>

namespace Ides {
namespace AST {
    
    llvm::Value* ASTStruct::GetValue(ParseContext &ctx) {
        this->type = Ides::Types::StructType::GetOrCreate(ctx, this->GetName());
        
        return NULL;
    }
    
    void ASTStruct::GenType(ParseContext &ctx) {
        if (this->members == NULL) return;
        
        std::vector<std::pair<Ides::String, const Ides::Types::Type*> > membertypes;
        for (auto i = this->members->begin(); i != this->members->end(); ++i) {
            if (ASTDeclaration* decl  = dynamic_cast<ASTDeclaration*>(*i)) {
                membertypes.push_back(std::make_pair(decl->name->name, decl->GetType(ctx)));
            }
        }
        this->type->SetMembers(ctx, membertypes);
    }
    

}
}