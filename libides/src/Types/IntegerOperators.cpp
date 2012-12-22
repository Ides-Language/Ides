#include <ides/Types/Type.h>
#include <ides/AST/AST.h>

namespace {
    typedef Ides::Types::ParseContext ParseContext;
    
    const Ides::Types::Type* IntPlusType(ParseContext& ctx, Ides::AST::AST* lhs, Ides::AST::AST* rhs) {
        const Ides::Types::Type* rhstype = rhs->GetType(ctx);
        const Ides::Types::Type* lhstype = lhs->GetType(ctx);
        if (const Ides::Types::NumberType* rhsnumtype = dynamic_cast<const Ides::Types::NumberType*>(rhstype)) {
            const Ides::Types::NumberType* lhsnumtype = dynamic_cast<const Ides::Types::NumberType*>(lhstype);
            if (rhsnumtype->IsEquivalentType(lhsnumtype))
                return lhsnumtype;
            else if (rhsnumtype->HasImplicitConversionTo(lhsnumtype))
                return lhsnumtype;
            else if (lhsnumtype->HasImplicitConversionTo(rhsnumtype))
                return rhsnumtype;
        }
        throw Ides::Diagnostics::CompileError("could not resolve operator + for rhs argument of type " + rhstype->ToString(), rhs->exprloc);
    }
    
    llvm::Value* IntPlusValue(ParseContext& ctx, Ides::AST::AST* lhs, Ides::AST::AST* rhs) {
        const Ides::Types::Type* rhstype = rhs->GetType(ctx);
        const Ides::Types::Type* lhstype = lhs->GetType(ctx);
        if (const Ides::Types::NumberType* rhsnumtype = dynamic_cast<const Ides::Types::NumberType*>(rhstype)) {
            const Ides::Types::NumberType* lhsnumtype = dynamic_cast<const Ides::Types::NumberType*>(lhstype);
            if (rhsnumtype->IsEquivalentType(lhsnumtype))
                return ctx.GetIRBuilder()->CreateAdd(lhs->GetValue(ctx), rhs->GetValue(ctx));
            else if (rhsnumtype->HasImplicitConversionTo(lhsnumtype))
                return ctx.GetIRBuilder()->CreateAdd(lhs->GetValue(ctx), rhsnumtype->Convert(ctx, rhs->GetValue(ctx), lhsnumtype));
            else if (lhsnumtype->HasImplicitConversionTo(rhsnumtype))
                return ctx.GetIRBuilder()->CreateAdd(lhsnumtype->Convert(ctx, lhs->GetValue(ctx), rhsnumtype), rhs->GetValue(ctx));
        }
        throw Ides::Diagnostics::CompileError("could not resolve operator + for rhs argument of type " + rhstype->ToString(), rhs->exprloc);
    }
}


namespace Ides {
namespace Types {
    
    void VoidType::InitAllBaseTypeMembers() {
        auto int_add = std::make_pair(IntPlusType, IntPlusValue);
        Integer8Type::GetSingletonPtr()->operators.insert(std::make_pair("+", int_add));
        UInteger8Type::GetSingletonPtr()->operators.insert(std::make_pair("+", int_add));
        Integer16Type::GetSingletonPtr()->operators.insert(std::make_pair("+", int_add));
        UInteger16Type::GetSingletonPtr()->operators.insert(std::make_pair("+", int_add));
        Integer32Type::GetSingletonPtr()->operators.insert(std::make_pair("+", int_add));
        UInteger32Type::GetSingletonPtr()->operators.insert(std::make_pair("+", int_add));
        Integer64Type::GetSingletonPtr()->operators.insert(std::make_pair("+", int_add));
        UInteger64Type::GetSingletonPtr()->operators.insert(std::make_pair("+", int_add));
    }
    
}
}