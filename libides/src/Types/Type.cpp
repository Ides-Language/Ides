#include <ides/Types/Type.h>


namespace Ides {
namespace Util {
#define SINGLETON(type) template<> type* Singleton<type>::msSingleton = new type()
    
    SINGLETON(Ides::Types::VoidType);
    SINGLETON(Ides::Types::UnitType);
    
    SINGLETON(Ides::Types::Integer8Type);
    SINGLETON(Ides::Types::Integer16Type);
    SINGLETON(Ides::Types::Integer32Type);
    SINGLETON(Ides::Types::Integer64Type);
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
    
    llvm::Type* FunctionType::GetLLVMType(llvm::IRBuilder<>* builder) const
    {
        std::vector<llvm::Type*> llvmargTypes;
        for (auto i = this->argTypes.begin(); i != this->argTypes.end(); ++i) {
            llvmargTypes.push_back((*i)->GetLLVMType(builder));
        }
        llvm::FunctionType *FT = llvm::FunctionType::get(retType->GetLLVMType(builder),llvmargTypes, false);
        return FT;
    }
    
}
    
}