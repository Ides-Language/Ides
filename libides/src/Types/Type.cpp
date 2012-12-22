#include <ides/Types/Type.h>
#include <ides/AST/AST.h>

namespace Ides {
namespace Util {
#define SINGLETON(type) template<> type* Singleton<type>::msSingleton = new type()
    
    
    SINGLETON(Ides::Types::Integer8Type);
    SINGLETON(Ides::Types::UInteger8Type);
    SINGLETON(Ides::Types::Integer16Type);
    SINGLETON(Ides::Types::UInteger16Type);
    SINGLETON(Ides::Types::Integer32Type);
    SINGLETON(Ides::Types::UInteger32Type);
    SINGLETON(Ides::Types::Integer64Type);
    SINGLETON(Ides::Types::UInteger64Type);
    
    SINGLETON(Ides::Types::Float32Type);
    SINGLETON(Ides::Types::Float64Type);
    
    SINGLETON(Ides::Types::VoidType);
    SINGLETON(Ides::Types::UnitType);
}
    
namespace Types {
    PointerType::PointerTypeMap PointerType::types;
    FunctionType::FunctionTypeSet FunctionType::types;
    
    const PointerType* PointerType::Get(const Ides::Types::Type* target) {
        auto i = types.find(target);
        if (i == types.end()) {
            PointerType* newptr = new PointerType(target);
            types.insert(std::make_pair(target, newptr));
            return newptr;
        }
        return i->second;
    }
    
    const FunctionType* FunctionType::Get(const Ides::Types::Type* retType, const std::vector<const Ides::Types::Type*>& argTypes)
    {
        for (auto i = types.begin(); i != types.end(); ++i) {
            if ((*i)->retType->IsEquivalentType(retType)) {
                auto thisai = (*i)->argTypes.begin();
                auto wantai = argTypes.begin();
                bool found = true;
                for (; thisai != (*i)->argTypes.end() && wantai != argTypes.end(); ++thisai, ++wantai) {
                    if (!(*thisai)->IsEquivalentType(*wantai)) {
                        found = false;
                        break;
                    }
                }
                if (found) return *i;
            }
        }
        
        FunctionType* f = new FunctionType(retType, argTypes);
        types.insert(f);
        return f;
    }
    
    llvm::Type* FunctionType::GetLLVMType(ParseContext& ctx) const
    {
        std::vector<llvm::Type*> llvmargTypes;
        for (auto i = this->argTypes.begin(); i != this->argTypes.end(); ++i) {
            llvmargTypes.push_back((*i)->GetLLVMType(ctx));
        }
        llvm::FunctionType *FT = llvm::FunctionType::get(retType->GetLLVMType(ctx),llvmargTypes, false);
        return FT;
    }

    
    
    const Ides::Types::Type* NumberType::GetOperatorType(const Ides::String& opname, ParseContext& ctx, Ides::AST::AST* lhs, Ides::AST::AST* rhs)
    {
        const Ides::Types::NumberType* lhstype = static_cast<const Ides::Types::NumberType*>(lhs->GetType(ctx));
        auto oper = lhstype->operators.find(opname);
        if (oper == lhstype->operators.end()) {
            throw Ides::Diagnostics::CompileError("no such operator exists on type " + lhstype->ToString(), lhs->exprloc);
        }
        return oper->second.first(ctx, lhs, rhs);
    }
    
    llvm::Value* NumberType::GetOperatorValue(const Ides::String& opname, ParseContext& ctx, Ides::AST::AST* lhs, Ides::AST::AST* rhs)
    {
        const Ides::Types::NumberType* lhstype = static_cast<const Ides::Types::NumberType*>(lhs->GetType(ctx));
        auto oper = lhstype->operators.find(opname);
        if (oper == lhstype->operators.end()) {
            throw Ides::Diagnostics::CompileError("no such operator exists on type " + lhstype->ToString(), lhs->exprloc);
        }
        return oper->second.second(ctx, lhs, rhs);
    }
    
    /** Numeric implicit conversions **/
    
    bool Float64Type::HasImplicitConversionTo(const Ides::Types::Type *other) const { return false; }
    bool Float32Type::HasImplicitConversionTo(const Ides::Types::Type *other) const { return other->IsEquivalentType(Float64Type::GetSingletonPtr()); }
    
    bool UInteger64Type::HasImplicitConversionTo(const Ides::Types::Type *other) const {
        return false;
    }
    bool Integer64Type::HasImplicitConversionTo(const Ides::Types::Type *other) const {
        return false;
    }
    
    bool UInteger32Type::HasImplicitConversionTo(const Ides::Types::Type *other) const {
        return UInteger64Type::GetSingletonPtr()->IsEquivalentType(other) || UInteger64Type::GetSingletonPtr()->HasImplicitConversionTo(other) ||
                Integer64Type::GetSingletonPtr()->IsEquivalentType(other) ||  Integer64Type::GetSingletonPtr()->HasImplicitConversionTo(other);
    }
    bool Integer32Type::HasImplicitConversionTo(const Ides::Types::Type *other) const {
        return Integer64Type::GetSingletonPtr()->IsEquivalentType(other) || Integer64Type::GetSingletonPtr()->HasImplicitConversionTo(other);
    }
    bool UInteger16Type::HasImplicitConversionTo(const Ides::Types::Type *other) const {
        return UInteger32Type::GetSingletonPtr()->IsEquivalentType(other) || UInteger32Type::GetSingletonPtr()->HasImplicitConversionTo(other) ||
        Integer32Type::GetSingletonPtr()->IsEquivalentType(other) || Integer32Type::GetSingletonPtr()->HasImplicitConversionTo(other);
    }
    bool Integer16Type::HasImplicitConversionTo(const Ides::Types::Type *other) const {
        return Integer32Type::GetSingletonPtr()->IsEquivalentType(other) || Integer32Type::GetSingletonPtr()->HasImplicitConversionTo(other);
    }
    bool UInteger8Type::HasImplicitConversionTo(const Ides::Types::Type *other) const {
        return UInteger16Type::GetSingletonPtr()->IsEquivalentType(other) || UInteger16Type::GetSingletonPtr()->HasImplicitConversionTo(other) ||
                Integer16Type::GetSingletonPtr()->IsEquivalentType(other) ||  Integer16Type::GetSingletonPtr()->HasImplicitConversionTo(other);
    }
    bool Integer8Type::HasImplicitConversionTo(const Ides::Types::Type *other) const {
        return Integer16Type::GetSingletonPtr()->IsEquivalentType(other) || Integer16Type::GetSingletonPtr()->HasImplicitConversionTo(other);
    }
    
}
    
}