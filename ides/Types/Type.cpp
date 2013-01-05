#include <ides/Types/Type.h>
#include <ides/AST/AST.h>

namespace Ides {
namespace Types {
    llvm::StringMap<const Ides::Types::Type*> Type::typenames;
    llvm::StringMap<Ides::Types::StructType*> StructType::types;
}
    
namespace Util {
#define SINGLETON(type) template<> type* Singleton<type>::msSingleton = new type()
    
    
    SINGLETON(Ides::Types::Integer1Type);
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
    
    SINGLETON(Ides::Types::OverloadedFunctionType);
}
    
namespace Types {
    PointerType::PointerTypeMap PointerType::types;
    FunctionType::FunctionTypeSet FunctionType::types;
    
    const Ides::Types::PointerType* Type::PtrType() const { return PointerType::Get(this); }
    
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
    
    StructType* StructType::GetOrCreate(ParseContext& ctx, Ides::StringRef name) {
        llvm::StringMap<Ides::Types::StructType*>::MapEntryTy& i = StructType::types.GetOrCreateValue(name);
        if (!i.getValue()) {
            StructType* t = new StructType(name);
            t->type = llvm::StructType::create(ctx.GetContext(), name);
            i.setValue(t);
        }
        return i.getValue();
    }
    
    void StructType::SetMembers(ParseContext& ctx, const std::vector<std::pair<Ides::String, const Type*> >& members) {
        this->type_members = members;
    }
    
    const Ides::Types::Type* StructType::GetMemberType(Ides::StringRef str) const {
        for (auto i = this->type_members.begin(); i != this->type_members.end(); ++i) {
            if (i->first == str) return i->second;
        }
        return NULL;
    }
    
    int StructType::GetMemberIndex(Ides::StringRef str) const {
        int ret = 0;
        for (auto i = this->type_members.begin(); i != this->type_members.end(); ++i) {
            if (i->first == str) return ret;
            ++ret;
        }
        return -1;
    }
    
    const Ides::String FunctionType::ToString() const {
        std::stringstream fntype;
        fntype << "fn(";
        auto i = this->argTypes.begin();
        
        if (i != this->argTypes.end()) fntype << (*i++)->ToString();
        for (; i != this->argTypes.end(); ++i) {
            fntype << ", " << (*i)->ToString();
        }
        fntype << "): " << this->retType->ToString();
        return fntype.str();
    }
    
    /** Numeric implicit conversions **/
    
    bool Float64Type::HasImplicitConversionTo(const Ides::Types::Type *other) const {
        return false;
    }
    bool Float32Type::HasImplicitConversionTo(const Ides::Types::Type *other) const {
        return other->IsEquivalentType(Float64Type::GetSingletonPtr());
    }
    
    bool UInteger64Type::HasImplicitConversionTo(const Ides::Types::Type *other) const {
        return false; //Float32Type::GetSingletonPtr()->IsEquivalentType(other) || Float32Type::GetSingletonPtr()->HasImplicitConversionTo(other);
    }
    bool Integer64Type::HasImplicitConversionTo(const Ides::Types::Type *other) const {
        return false; //Float32Type::GetSingletonPtr()->IsEquivalentType(other) || Float32Type::GetSingletonPtr()->HasImplicitConversionTo(other);
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
    
    bool Integer1Type::HasImplicitConversionTo(const Ides::Types::Type *other) const {
        return false;
    }
    
}
    
}