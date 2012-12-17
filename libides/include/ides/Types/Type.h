#ifndef _IDES_TYPES_H_
#define _IDES_TYPES_H_

#include <ides/common.h>

namespace Ides {
namespace Types {

    
    class Type {
    public:
        Type(Type* supertype) : supertype(supertype) { }
        
        virtual llvm::Type* GetLLVMType(llvm::IRBuilder<>* builder) const { throw std::runtime_error("LLVM type not yet implemented."); }
        
        virtual bool IsSupertypeOf(const Type* other) const {
            if (this->IsEquivalentType(other)) return true;
            return (supertype != NULL) && this->supertype->IsSupertypeOf(other);
        }
        
        virtual bool IsSubTypeOf(const Type* other) const {
            return other->IsSupertypeOf(this);
        }
        
        virtual bool IsEquivalentType(const Type* other) const {
            return other == this;
        }
        
        
    protected:
        const Type* supertype;
    };
    
    class VoidType : public Type, public Ides::Util::Singleton<VoidType> {
    public:
        VoidType() : Type(NULL) { }
        
        virtual llvm::Type* GetLLVMType(llvm::IRBuilder<>* builder) const {
            return llvm::Type::getVoidTy(builder->getContext());
        }
        virtual bool IsSupertypeOf(const Type* other) const {
            return true; // VoidType is a supertype of all other types.
        }
        virtual bool IsSubtypeOf(const Type* other) const {
            return other == this; // VoidType is the universal subtype
        }
    };
    
    class UnitType : public Type {
    public:
        UnitType() : Type(NULL) { }
        virtual llvm::Type* GetLLVMType(llvm::IRBuilder<>* builder) const {
            throw std::runtime_error("Unit type has no LLVM type."); // Unit has no LLVM type.
        }
        
        virtual bool IsSupertypeOf(const Type* other) const {
            return false; // Nothing inherits from Unit.
        }
        
        virtual bool IsSubtypeOf(const Type* other) const {
            return true; // Unit is a subtype of all other types.
        }
        
    };
    
    class FunctionType : public Type {
        FunctionType(const Ides::Types::Type* retType, const std::vector<const Ides::Types::Type*>& argTypes) :
            Type(VoidType::GetSingletonPtr()), retType(retType), argTypes(argTypes) { }
    public:
        typedef boost::unordered_set<FunctionType*> FunctionTypeSet;
        static const FunctionType* Get(const Ides::Types::Type* retType, const std::vector<const Ides::Types::Type*>& argTypes);
        
        virtual llvm::Type* GetLLVMType(llvm::IRBuilder<>* builder) const;

        const Ides::Types::Type* retType;
        const std::vector<const Ides::Types::Type*> argTypes;
        
    private:
        static FunctionTypeSet types;
        llvm::FunctionType* ft;
    };
    
    class PointerType : public Type {
        PointerType(const Ides::Types::Type* targetType) : Type(VoidType::GetSingletonPtr()), targetType(targetType) { }
    public:
        typedef boost::unordered_map<const Ides::Types::Type*, PointerType*> PointerTypeMap;
        static const PointerType* Get(const Ides::Types::Type* target);
        
        virtual llvm::Type* GetLLVMType(llvm::IRBuilder<>* builder) const { return llvm::PointerType::getUnqual(targetType->GetLLVMType(builder)); }
    private:
        const Ides::Types::Type* targetType;
        static PointerTypeMap types;
    };
    
    
    class ClassType : public Type {
    public:
        
    };
    
    class ReferenceType : public Type {
    public:
        
    };
    
    class StringType : public ReferenceType {
    public:
        
    };
    
#define IntegerType(size) \
    class Integer##size##Type : public Type, public Ides::Util::Singleton<Integer##size##Type> { \
    public: \
    Integer##size##Type() : Type(VoidType::GetSingletonPtr()) { } \
        virtual llvm::Type* GetLLVMType(llvm::IRBuilder<>* builder) const { \
            return llvm::Type::getInt##size##Ty(builder->getContext()); \
        } \
    }
    
    IntegerType(8);
    IntegerType(16);
    IntegerType(32);
    IntegerType(64);
    
} // namespace Types
} // namespace Ides
#endif // _IDES_TYPES_H_

