#include <ides/AST/AST.h>
#include <ides/AST/Expression.h>
#include <ides/AST/ConstantExpression.h>
#include <ides/AST/Declaration.h>

#include <boost/uuid/uuid_generators.hpp> // generators
#include <boost/uuid/uuid_io.hpp>         // streaming operators etc.

#include <sstream>

namespace Ides {
namespace AST {
    
    const Ides::Types::Type* FunctionType::GetType(ASTContext& ctx) {
        std::vector<const Ides::Types::Type*> argTypes;
        const Ides::Types::Type* rtype = this->rettype ? this->rettype->GetType(ctx) : Ides::Types::VoidType::GetSingletonPtr();
        for (auto i = this->argtypes.begin(); i != this->argtypes.end(); ++i) {
            argTypes.push_back((*i)->GetType(ctx));
        }
        return Ides::Types::FunctionType::Get(rtype, argTypes);
    }
    
    const Ides::Types::Type* TypeName::GetType(Ides::AST::ASTContext &ctx) {
        Ides::String typen = **this->name;
        Ides::AST::Declaration* decl = ctx.GetCurrentScope()->GetMember(ctx, typen);
        if (decl) {
            return decl->GetType(ctx);
        }
        return NULL;
    }
}
}