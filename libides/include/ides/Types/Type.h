#ifndef _IDES_TYPES_H_
#define _IDES_TYPES_H_

#include <ides/common.h>
#include <ides/Parsing/Parser.h>
namespace Ides {
namespace Types {

    typedef Ides::Parsing::Parser ParseContext;
    
    class Type {
    public:
        Type(const Ides::String& type_name, Type* supertype) : type_name(type_name), supertype(supertype) { }
        
        virtual llvm::Type* GetLLVMType(ParseContext& ctx) const { throw std::runtime_error("LLVM type not yet implemented."); }
        
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
        
        const Ides::String type_name;
    protected:
        const Type* supertype;
    };
    
    class VoidType : public Type, public Ides::Util::Singleton<VoidType> {
    public:
        VoidType() : Type("void", NULL) { }
        
        virtual llvm::Type* GetLLVMType(ParseContext& ctx) const {
            return llvm::Type::getVoidTy(ctx.GetIRBuilder()->getContext());
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
        UnitType() : Type("unit", NULL) { }
        virtual llvm::Type* GetLLVMType(ParseContext& ctx) const {
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
            Type("() : void", VoidType::GetSingletonPtr()), retType(retType), argTypes(argTypes) { }
    public:
        typedef boost::unordered_set<FunctionType*> FunctionTypeSet;
        static const FunctionType* Get(const Ides::Types::Type* retType, const std::vector<const Ides::Types::Type*>& argTypes);
        
        virtual llvm::Type* GetLLVMType(ParseContext& ctx) const;

        const Ides::Types::Type* retType;
        const std::vector<const Ides::Types::Type*> argTypes;
        
    private:
        static FunctionTypeSet types;
        llvm::FunctionType* ft;
    };
    
    class PointerType : public Type {
        PointerType(const Ides::Types::Type* targetType) : Type(targetType->type_name + "*", VoidType::GetSingletonPtr()), targetType(targetType) { }
    public:
        typedef boost::unordered_map<const Ides::Types::Type*, PointerType*> PointerTypeMap;
        static const PointerType* Get(const Ides::Types::Type* target);
        
        virtual llvm::Type* GetLLVMType(ParseContext& ctx) const {
            return llvm::PointerType::getUnqual(targetType->GetLLVMType(ctx));
        }
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
        Integer##size##Type() : Type("int" #size, VoidType::GetSingletonPtr()) { } \
        ~Integer##size##Type() { } \
        virtual llvm::Type* GetLLVMType(ParseContext& ctx) const { \
            return llvm::Type::getInt##size##Ty(ctx.GetIRBuilder()->getContext()); \
        } \
    }
    
#define UIntegerType(size) \
    class UInteger##size##Type : public Type, public Ides::Util::Singleton<UInteger##size##Type> { \
    public: \
        UInteger##size##Type() : Type("uint" #size, VoidType::GetSingletonPtr()) { } \
        ~UInteger##size##Type() { } \
        virtual llvm::Type* GetLLVMType(ParseContext& ctx) const { \
            return llvm::Type::getInt##size##Ty(ctx.GetIRBuilder()->getContext()); \
        } \
    }
    
    IntegerType(8);
    UIntegerType(8);
    IntegerType(16);
    UIntegerType(16);
    IntegerType(32);
    UIntegerType(32);
    IntegerType(64);
    UIntegerType(64);
    
    class Float32Type : public Type, public Ides::Util::Singleton<Float32Type> {
    public:
        Float32Type() : Type("float32", VoidType::GetSingletonPtr()) { }
        virtual ~Float32Type() { }
        virtual llvm::Type* GetLLVMTYpe(ParseContext& ctx) const {
            return llvm::Type::getFloatTy(ctx.GetIRBuilder()->getContext());
        }
    };
    
    class Float64Type : public Type, public Ides::Util::Singleton<Float64Type> {
    public:
        Float64Type() : Type("float64", VoidType::GetSingletonPtr()) { }
        virtual ~Float64Type() { }
        virtual llvm::Type* GetLLVMTYpe(ParseContext& ctx) const {
            return llvm::Type::getDoubleTy(ctx.GetIRBuilder()->getContext());
        }
    };
    
} // namespace Types
} // namespace Ides
#endif // _IDES_TYPES_H_

