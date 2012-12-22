#include <ides/Types/Type.h>
#include <ides/AST/AST.h>

namespace {
    typedef Ides::Types::ParseContext ParseContext;
    
    template<char op>
    const Ides::Types::Type* IntArithmeticType(ParseContext& ctx, Ides::AST::AST* lhs, Ides::AST::AST* rhs) {
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
        std::stringstream err;
        err << "could not resolve operator " << op << " for rhs argument of type " << rhstype->ToString();
        throw Ides::Diagnostics::CompileError(err.str(), rhs->exprloc);
    }
    
#define MAKE_INT_ARITHMETIC_OPERATOR_VALUE(name, op, method) \
    llvm::Value* Int##name##Value(ParseContext& ctx, Ides::AST::AST* lhs, Ides::AST::AST* rhs) { \
        const Ides::Types::Type* rhstype = rhs->GetType(ctx); \
        const Ides::Types::Type* lhstype = lhs->GetType(ctx); \
        if (const Ides::Types::NumberType* rhsnumtype = dynamic_cast<const Ides::Types::NumberType*>(rhstype)) { \
            const Ides::Types::NumberType* lhsnumtype = dynamic_cast<const Ides::Types::NumberType*>(lhstype); \
            if (rhsnumtype->IsEquivalentType(lhsnumtype)) return ctx.GetIRBuilder()->method(lhs->GetValue(ctx), rhs->GetValue(ctx)); \
            else if (rhsnumtype->HasImplicitConversionTo(lhsnumtype)) return ctx.GetIRBuilder()->method(lhs->GetValue(ctx), rhsnumtype->Convert(ctx, rhs->GetValue(ctx), lhsnumtype)); \
            else if (lhsnumtype->HasImplicitConversionTo(rhsnumtype)) return ctx.GetIRBuilder()->method(lhsnumtype->Convert(ctx, lhs->GetValue(ctx), rhsnumtype), rhs->GetValue(ctx)); \
        } \
        throw Ides::Diagnostics::CompileError("could not resolve operator " op " for rhs argument of type ", rhs->exprloc); \
    }
    
    MAKE_INT_ARITHMETIC_OPERATOR_VALUE(Plus, "+", CreateAdd);
    MAKE_INT_ARITHMETIC_OPERATOR_VALUE(Sub, "-", CreateSub);
    MAKE_INT_ARITHMETIC_OPERATOR_VALUE(Mul, "*", CreateMul);
    
    llvm::Value* IntDivideValue(ParseContext& ctx, Ides::AST::AST* lhs, Ides::AST::AST* rhs) {
        const Ides::Types::Type* rhstype = rhs->GetType(ctx);
        const Ides::Types::Type* lhstype = lhs->GetType(ctx);
        if (const Ides::Types::NumberType* rhsnumtype = dynamic_cast<const Ides::Types::NumberType*>(rhstype)) {
            const Ides::Types::NumberType* lhsnumtype = dynamic_cast<const Ides::Types::NumberType*>(lhstype);
            if (rhsnumtype->IsEquivalentType(lhsnumtype))
                return rhsnumtype->IsSigned() ?
                    ctx.GetIRBuilder()->CreateSDiv(lhs->GetValue(ctx), rhs->GetValue(ctx)) :
                    ctx.GetIRBuilder()->CreateUDiv(lhs->GetValue(ctx), rhs->GetValue(ctx));
            else if (rhsnumtype->HasImplicitConversionTo(lhsnumtype))
                return lhsnumtype->IsSigned() ?
                ctx.GetIRBuilder()->CreateSDiv(lhs->GetValue(ctx), rhsnumtype->Convert(ctx, rhs->GetValue(ctx), lhsnumtype)) :
                ctx.GetIRBuilder()->CreateUDiv(lhs->GetValue(ctx), rhsnumtype->Convert(ctx, rhs->GetValue(ctx), lhsnumtype));
            else if (lhsnumtype->HasImplicitConversionTo(rhsnumtype))
                return lhsnumtype->IsSigned() ?
                    ctx.GetIRBuilder()->CreateSDiv(lhsnumtype->Convert(ctx, lhs->GetValue(ctx), rhsnumtype), rhs->GetValue(ctx)) :
                    ctx.GetIRBuilder()->CreateUDiv(lhsnumtype->Convert(ctx, lhs->GetValue(ctx), rhsnumtype), rhs->GetValue(ctx));
        }
        throw Ides::Diagnostics::CompileError("could not resolve operator / for rhs argument of type ", rhs->exprloc);
    }
}

#define MAKE_INT_ARITHMETIC_OPERATOR(op, pair) \
    Integer8Type::GetSingletonPtr()->operators.insert(std::make_pair(op, pair)); \
    UInteger8Type::GetSingletonPtr()->operators.insert(std::make_pair(op, pair)); \
    Integer16Type::GetSingletonPtr()->operators.insert(std::make_pair(op, pair)); \
    UInteger16Type::GetSingletonPtr()->operators.insert(std::make_pair(op, pair)); \
    Integer32Type::GetSingletonPtr()->operators.insert(std::make_pair(op, pair)); \
    UInteger32Type::GetSingletonPtr()->operators.insert(std::make_pair(op, pair)); \
    Integer64Type::GetSingletonPtr()->operators.insert(std::make_pair(op, pair)); \
    UInteger64Type::GetSingletonPtr()->operators.insert(std::make_pair(op, pair)); \


namespace Ides {
namespace Types {
    
    void VoidType::InitAllBaseTypeMembers() {
        auto int_add = std::make_pair(IntArithmeticType<'+'>, IntPlusValue);
        auto int_subtract = std::make_pair(IntArithmeticType<'-'>, IntSubValue);
        auto int_times = std::make_pair(IntArithmeticType<'*'>, IntMulValue);
        auto int_divide = std::make_pair(IntArithmeticType<'/'>, IntDivideValue);
        
        MAKE_INT_ARITHMETIC_OPERATOR("+", int_add)
        MAKE_INT_ARITHMETIC_OPERATOR("-", int_subtract)
        MAKE_INT_ARITHMETIC_OPERATOR("*", int_times)
        MAKE_INT_ARITHMETIC_OPERATOR("/", int_divide)
    }
    
}
}