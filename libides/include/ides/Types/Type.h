#ifndef _IDES_TYPES_H_
#define _IDES_TYPES_H_

#include <ides/common.h>
#include <ides/Parsing/Parser.h>
namespace Ides {
namespace Types {

    typedef Ides::Parsing::Parser ParseContext;
    
    class Type {
    public:
        Type(const Ides::String& type_name, Type* supertype) : type_name(type_name), supertype(supertype) {
            if (supertype)
                this->symbols.reset(new Ides::Parsing::SymbolTable(supertype->symbols));
            else
                this->symbols.reset(new Ides::Parsing::SymbolTable());
        }
        
        virtual llvm::Type* GetLLVMType(ParseContext& ctx) const { assert(0); throw std::runtime_error("LLVM type not yet implemented."); }
        
        virtual bool IsSupertypeOf(const Type* other) const {
            if (this->IsEquivalentType(other)) return true;
            return (supertype != NULL) && this->supertype->IsSupertypeOf(other);
        }
        
        virtual bool IsSubtypeOf(const Type* other) const {
            return other->IsSupertypeOf(this);
        }
        
        virtual bool IsEquivalentType(const Type* other) const {
            return other == this;
        }
        
        virtual bool HasImplicitConversionTo(const Type* other) const {
            return IsSubtypeOf(other);
        }
        
        virtual llvm::Value* Convert(Ides::Parsing::Parser& ctx, llvm::Value* val, const Type* to) const {
            std::stringstream err;
            err << "no conversion to type " << to->ToString() << " exists.";
            throw std::runtime_error(err.str());
        }
        
        const Ides::String ToString() const { return type_name; }
        
    protected:
        Ides::Parsing::SymbolTable::Ptr symbols;
        const Ides::String type_name;
        const Type* supertype;
    };
    
    class VoidType : public Type, public Ides::Util::Singleton<VoidType> {
    public:
        VoidType() : Type("void", NULL) { InitAllBaseTypeMembers(); }
        
        virtual llvm::Type* GetLLVMType(ParseContext& ctx) const {
            return llvm::Type::getVoidTy(ctx.GetIRBuilder()->getContext());
        }
        virtual bool IsSupertypeOf(const Type* other) const {
            return true; // VoidType is a supertype of all other types.
        }
        virtual bool IsSubtypeOf(const Type* other) const {
            return false; // VoidType is the universal subtype
        }
    private:
        static void InitAllBaseTypeMembers();
    };
    
    class UnitType : public Type {
    public:
        UnitType() : Type("unit", NULL) { }
        virtual llvm::Type* GetLLVMType(ParseContext& ctx) const {
            // Expressions of type Unit do not return. Use void as a placeholder.
            return llvm::Type::getVoidTy(ctx.GetIRBuilder()->getContext());
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
        PointerType(const Ides::Types::Type* targetType) : Type(targetType->ToString() + "*", VoidType::GetSingletonPtr()), targetType(targetType) { }
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
    
    class NumberType : public Type {
    public:
        NumberType(const Ides::String& type_name, Type* supertype) : Type(type_name, supertype) { }
        virtual ~NumberType() { }
        
        virtual bool IsSigned() const = 0;
        virtual uint8_t GetSize() const = 0;
    };
    
#define IntegerType(size) \
    class Integer##size##Type : public NumberType, public Ides::Util::Singleton<Integer##size##Type> { \
    public: \
        Integer##size##Type() : NumberType("int" #size, VoidType::GetSingletonPtr()) { } \
        ~Integer##size##Type() { } \
        virtual llvm::Type* GetLLVMType(ParseContext& ctx) const { \
            return llvm::Type::getInt##size##Ty(ctx.GetIRBuilder()->getContext()); \
        } \
        virtual bool IsSigned() const { return true; } \
        virtual uint8_t GetSize() const { return size; } \
        virtual bool HasImplicitConversionTo(const Type* other) const; \
        virtual llvm::Value* Convert(Ides::Parsing::Parser& ctx, llvm::Value* val, const Type* to) const { \
            const NumberType* t = dynamic_cast<const NumberType*>(to); assert(t); \
            return ctx.GetIRBuilder()->CreateIntCast(val, to->GetLLVMType(ctx), t->IsSigned(), "impconv");\
        }\
    }

#define UIntegerType(size) \
    class UInteger##size##Type : public NumberType, public Ides::Util::Singleton<UInteger##size##Type> { \
    public: \
        UInteger##size##Type() : NumberType("uint" #size, VoidType::GetSingletonPtr()) { } \
        ~UInteger##size##Type() { } \
        virtual llvm::Type* GetLLVMType(ParseContext& ctx) const { \
            return llvm::Type::getInt##size##Ty(ctx.GetIRBuilder()->getContext()); \
        } \
        virtual bool IsSigned() const { return false; } \
        virtual uint8_t GetSize() const { return size; } \
        virtual bool HasImplicitConversionTo(const Type* other) const; \
        virtual llvm::Value* Convert(Ides::Parsing::Parser& ctx, llvm::Value* val, const Type* to) const { \
            const NumberType* t = dynamic_cast<const NumberType*>(to); assert(t); \
            return ctx.GetIRBuilder()->CreateIntCast(val, to->GetLLVMType(ctx), t->IsSigned(), "impconv");\
        }\
    }
    
    IntegerType(8);
    UIntegerType(8);
    IntegerType(16);
    UIntegerType(16);
    IntegerType(32);
    UIntegerType(32);
    IntegerType(64);
    UIntegerType(64);
    
    class Float32Type : public NumberType, public Ides::Util::Singleton<Float32Type> {
    public:
        Float32Type() : NumberType("float32", VoidType::GetSingletonPtr()) { }
        virtual ~Float32Type() { }
        virtual llvm::Type* GetLLVMType(ParseContext& ctx) const {
            return llvm::Type::getFloatTy(ctx.GetIRBuilder()->getContext());
        }
        virtual bool IsSigned() const { return true; }
        virtual uint8_t GetSize() const { return 32; }
        virtual bool HasImplicitConversionTo(const Type* other) const;
    };
    
    class Float64Type : public NumberType, public Ides::Util::Singleton<Float64Type> {
    public:
        Float64Type() : NumberType("float64", VoidType::GetSingletonPtr()) { }
        virtual ~Float64Type() { }
        virtual llvm::Type* GetLLVMType(ParseContext& ctx) const {
            return llvm::Type::getDoubleTy(ctx.GetIRBuilder()->getContext());
        }
        virtual bool IsSigned() const { return true; }
        virtual uint8_t GetSize() const { return 64; }
        virtual bool HasImplicitConversionTo(const Type* other) const;
    };
    
} // namespace Types
} // namespace Ides
#endif // _IDES_TYPES_H_

