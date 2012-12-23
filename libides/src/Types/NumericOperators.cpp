#include <ides/Types/Type.h>
#include <ides/AST/AST.h>

namespace {
    typedef Ides::Types::ParseContext ParseContext;
    
    const Ides::Types::Type* NumericArithmeticType(ParseContext& ctx, Ides::AST::AST* lhs, Ides::AST::AST* rhs) {
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
            else
                throw Ides::Diagnostics::CompileError("no implicit conversion for rhs argument", lhs->exprloc + rhs->exprloc);
        }
        std::stringstream err;
        err << "could not resolve operator for rhs argument of type " << rhstype->ToString();
        throw Ides::Diagnostics::CompileError(err.str(), lhs->exprloc + rhs->exprloc);
    }
    
#define MAKE_NUM_ARITHMETIC_OPERATOR_VALUE(name, op, method) \
    llvm::Value* name##Value(ParseContext& ctx, Ides::AST::AST* lhs, Ides::AST::AST* rhs) { \
        const Ides::Types::Type* rhstype = rhs->GetType(ctx); \
        const Ides::Types::Type* lhstype = lhs->GetType(ctx); \
        if (const Ides::Types::NumberType* rhsnumtype = dynamic_cast<const Ides::Types::NumberType*>(rhstype)) { \
            const Ides::Types::NumberType* lhsnumtype = dynamic_cast<const Ides::Types::NumberType*>(lhstype); \
            if (rhsnumtype->IsEquivalentType(lhsnumtype)) return ctx.GetIRBuilder()->method(lhs->GetValue(ctx), rhs->GetValue(ctx)); \
            else if (rhsnumtype->HasImplicitConversionTo(lhsnumtype)) return ctx.GetIRBuilder()->method(lhs->GetValue(ctx), rhsnumtype->Convert(ctx, rhs->GetValue(ctx), lhsnumtype)); \
            else if (lhsnumtype->HasImplicitConversionTo(rhsnumtype)) return ctx.GetIRBuilder()->method(lhsnumtype->Convert(ctx, lhs->GetValue(ctx), rhsnumtype), rhs->GetValue(ctx)); \
        } \
        throw Ides::Diagnostics::CompileError("could not resolve operator " op " for rhs argument of type " + rhstype->ToString(), rhs->exprloc); \
    }
    
    MAKE_NUM_ARITHMETIC_OPERATOR_VALUE(IntPlus, "+", CreateAdd);
    MAKE_NUM_ARITHMETIC_OPERATOR_VALUE(IntSub, "-", CreateSub);
    MAKE_NUM_ARITHMETIC_OPERATOR_VALUE(IntMul, "*", CreateMul);
    
    MAKE_NUM_ARITHMETIC_OPERATOR_VALUE(FltPlus, "+", CreateFAdd);
    MAKE_NUM_ARITHMETIC_OPERATOR_VALUE(FltSub, "-", CreateFSub);
    MAKE_NUM_ARITHMETIC_OPERATOR_VALUE(FltMul, "*", CreateFMul);
    MAKE_NUM_ARITHMETIC_OPERATOR_VALUE(FltDiv, "/", CreateFDiv);
    MAKE_NUM_ARITHMETIC_OPERATOR_VALUE(FltMod, "*", CreateFRem);
    
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
        throw Ides::Diagnostics::CompileError("could not resolve operator / for rhs argument of type " + rhstype->ToString(), rhs->exprloc);
    }
    
    llvm::Value* IntModValue(ParseContext& ctx, Ides::AST::AST* lhs, Ides::AST::AST* rhs) {
        const Ides::Types::Type* rhstype = rhs->GetType(ctx);
        const Ides::Types::Type* lhstype = lhs->GetType(ctx);
        if (const Ides::Types::NumberType* rhsnumtype = dynamic_cast<const Ides::Types::NumberType*>(rhstype)) {
            const Ides::Types::NumberType* lhsnumtype = dynamic_cast<const Ides::Types::NumberType*>(lhstype);
            if (rhsnumtype->IsEquivalentType(lhsnumtype))
                return rhsnumtype->IsSigned() ?
                ctx.GetIRBuilder()->CreateSRem(lhs->GetValue(ctx), rhs->GetValue(ctx)) :
                ctx.GetIRBuilder()->CreateURem(lhs->GetValue(ctx), rhs->GetValue(ctx));
            else if (rhsnumtype->HasImplicitConversionTo(lhsnumtype))
                return lhsnumtype->IsSigned() ?
                ctx.GetIRBuilder()->CreateSRem(lhs->GetValue(ctx), rhsnumtype->Convert(ctx, rhs->GetValue(ctx), lhsnumtype)) :
                ctx.GetIRBuilder()->CreateURem(lhs->GetValue(ctx), rhsnumtype->Convert(ctx, rhs->GetValue(ctx), lhsnumtype));
            else if (lhsnumtype->HasImplicitConversionTo(rhsnumtype))
                return lhsnumtype->IsSigned() ?
                ctx.GetIRBuilder()->CreateSRem(lhsnumtype->Convert(ctx, lhs->GetValue(ctx), rhsnumtype), rhs->GetValue(ctx)) :
                ctx.GetIRBuilder()->CreateURem(lhsnumtype->Convert(ctx, lhs->GetValue(ctx), rhsnumtype), rhs->GetValue(ctx));
        }
        throw Ides::Diagnostics::CompileError("could not resolve operator % for rhs argument of type " + rhstype->ToString(), rhs->exprloc);
    }
    
    template<int Pred>
    llvm::Value* Cmp(ParseContext& ctx, Ides::AST::AST* lhs, Ides::AST::AST* rhs) {
        const Ides::Types::Type* rhstype = rhs->GetType(ctx);
        const Ides::Types::Type* lhstype = lhs->GetType(ctx);
        if (const Ides::Types::NumberType* rhsnumtype = dynamic_cast<const Ides::Types::NumberType*>(rhstype)) {
            const Ides::Types::NumberType* lhsnumtype = dynamic_cast<const Ides::Types::NumberType*>(lhstype);
            if (rhsnumtype->IsEquivalentType(lhsnumtype))
                return ctx.GetIRBuilder()->CreateICmp((llvm::CmpInst::Predicate)Pred, lhs->GetValue(ctx), rhs->GetValue(ctx));
            else if (rhsnumtype->HasImplicitConversionTo(lhsnumtype))
                return ctx.GetIRBuilder()->CreateICmp((llvm::CmpInst::Predicate)Pred,lhs->GetValue(ctx), rhsnumtype->Convert(ctx, rhs->GetValue(ctx), lhsnumtype));
            else if (lhsnumtype->HasImplicitConversionTo(rhsnumtype))
                return ctx.GetIRBuilder()->CreateICmp((llvm::CmpInst::Predicate)Pred,lhsnumtype->Convert(ctx, lhs->GetValue(ctx), rhsnumtype), rhs->GetValue(ctx));
        }
        throw Ides::Diagnostics::CompileError("could not resolve operator for rhs argument of type " + rhstype->ToString(), rhs->exprloc);
    }
}

#define MAKE_SIGNED_INT_ARITHMETIC_OPERATOR(op, pair) \
    Integer8Type::GetSingletonPtr()->operators.insert(std::make_pair(op, pair)); \
    Integer16Type::GetSingletonPtr()->operators.insert(std::make_pair(op, pair)); \
    Integer32Type::GetSingletonPtr()->operators.insert(std::make_pair(op, pair)); \
    Integer64Type::GetSingletonPtr()->operators.insert(std::make_pair(op, pair)); \

#define MAKE_UNSIGNED_INT_ARITHMETIC_OPERATOR(op, pair) \
    UInteger8Type::GetSingletonPtr()->operators.insert(std::make_pair(op, pair)); \
    UInteger16Type::GetSingletonPtr()->operators.insert(std::make_pair(op, pair)); \
    UInteger32Type::GetSingletonPtr()->operators.insert(std::make_pair(op, pair)); \
    UInteger64Type::GetSingletonPtr()->operators.insert(std::make_pair(op, pair)); \


#define MAKE_INT_ARITHMETIC_OPERATOR(op, pair) \
    MAKE_SIGNED_INT_ARITHMETIC_OPERATOR(op, pair) \
    MAKE_UNSIGNED_INT_ARITHMETIC_OPERATOR(op, pair)

#define MAKE_FLT_ARITHMETIC_OPERATOR(op, pair) \
    Float32Type::GetSingletonPtr()->operators.insert(std::make_pair(op, pair)); \
    Float64Type::GetSingletonPtr()->operators.insert(std::make_pair(op, pair)); \

#define MAKE_SINT_CMP(op, pred) MAKE_SIGNED_INT_ARITHMETIC_OPERATOR(op, std::make_pair(NumericArithmeticType, Cmp<llvm::CmpInst::pred>));
#define MAKE_UINT_CMP(op, pred) MAKE_SIGNED_INT_ARITHMETIC_OPERATOR(op, std::make_pair(NumericArithmeticType, Cmp<llvm::CmpInst::pred>));
#define MAKE_INT_CMP(op, pred) \
    MAKE_SINT_CMP(op, pred) \
    MAKE_UINT_CMP(op, pred)

namespace Ides {
namespace Types {
    
    void VoidType::InitAllBaseTypeMembers() {
        auto int_add = std::make_pair(NumericArithmeticType, IntPlusValue);
        auto int_subtract = std::make_pair(NumericArithmeticType, IntSubValue);
        auto int_times = std::make_pair(NumericArithmeticType, IntMulValue);
        auto int_divide = std::make_pair(NumericArithmeticType, IntDivideValue);
        auto int_mod = std::make_pair(NumericArithmeticType, IntModValue);
        
        auto flt_add = std::make_pair(NumericArithmeticType, FltPlusValue);
        auto flt_subtract = std::make_pair(NumericArithmeticType, FltSubValue);
        auto flt_times = std::make_pair(NumericArithmeticType, FltMulValue);
        auto flt_divide = std::make_pair(NumericArithmeticType, FltDivValue);
        auto flt_mod = std::make_pair(NumericArithmeticType, FltModValue);
        
        MAKE_INT_ARITHMETIC_OPERATOR("+", int_add)
        MAKE_INT_ARITHMETIC_OPERATOR("-", int_subtract)
        MAKE_INT_ARITHMETIC_OPERATOR("*", int_times)
        MAKE_INT_ARITHMETIC_OPERATOR("/", int_divide)
        MAKE_INT_ARITHMETIC_OPERATOR("%", int_mod)
        
        MAKE_INT_CMP("==", ICMP_EQ)
        MAKE_INT_CMP("!=", ICMP_NE)
        
        MAKE_SINT_CMP("<", ICMP_SLT)
        MAKE_SINT_CMP(">", ICMP_SGT)
        MAKE_SINT_CMP("<=", ICMP_SLE)
        MAKE_SINT_CMP(">=", ICMP_SGE)
        
        MAKE_UINT_CMP("<", ICMP_ULT)
        MAKE_UINT_CMP(">", ICMP_UGT)
        MAKE_UINT_CMP("<=", ICMP_ULE)
        MAKE_UINT_CMP(">=", ICMP_UGE)
        
        MAKE_FLT_ARITHMETIC_OPERATOR("+", flt_add)
        MAKE_FLT_ARITHMETIC_OPERATOR("-", flt_subtract)
        MAKE_FLT_ARITHMETIC_OPERATOR("*", flt_times)
        MAKE_FLT_ARITHMETIC_OPERATOR("/", flt_divide)
        MAKE_FLT_ARITHMETIC_OPERATOR("%", flt_mod)
    }
    
}
}