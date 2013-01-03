#ifndef _IDES_TYPES_H_
#define _IDES_TYPES_H_

#include <ides/common.h>
#include <ides/AST/ASTContext.h>
#include <boost/function.hpp>
#include <boost/unordered_map.hpp>
#include <ides/AST/DeclarationContext.h>
namespace Ides {
    namespace AST {
        class AST;
        class Declaration;
    }
namespace Types {

    typedef Ides::AST::ASTContext ParseContext;
    
    class PointerType;
    
    class Type : public Ides::AST::ConcreteDeclarationContext {
    public:
        Type(Ides::StringRef type_name, Type* supertype) : type_name(type_name), supertype(supertype) {
            typenames.GetOrCreateValue(type_name, this);
        }
        
        virtual llvm::Type* GetLLVMType(ParseContext& ctx) const { assert(0); throw std::runtime_error("LLVM type not yet implemented."); }
        
        static const Ides::Types::Type* GetFromMDNode(const llvm::Value* node);
        
        const Ides::Types::PointerType* PtrType() const;
        
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
        
        
        virtual const Ides::String ToString() const { return type_name; }
        
    protected:
        const Ides::String type_name;
        const Type* supertype;
        
        static llvm::StringMap<const Ides::Types::Type*> typenames;
    };
    
    class VoidType : public Type, public Ides::Util::Singleton<VoidType> {
    public:
        VoidType() : Type("void", NULL) { }
        
        virtual llvm::Type* GetLLVMType(ParseContext& ctx) const {
            return llvm::Type::getVoidTy(ctx.GetContext());
        }
        virtual bool IsSupertypeOf(const Type* other) const {
            return true; // VoidType is a supertype of all other types.
        }
        virtual bool IsSubtypeOf(const Type* other) const {
            return false; // VoidType is the universal subtype
        }
    };
    
    class UnitType : public Type, public Ides::Util::Singleton<UnitType> {
    public:
        UnitType() : Type("unit", NULL) { }
        virtual llvm::Type* GetLLVMType(ParseContext& ctx) const {
            // Expressions of type Unit do not return. Use void as a placeholder.
            return llvm::Type::getVoidTy(ctx.GetContext());
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
        
        virtual const Ides::String ToString() const;

        const Ides::Types::Type* retType;
        const std::vector<const Ides::Types::Type*> argTypes;
        
    private:
        static FunctionTypeSet types;
        llvm::FunctionType* ft;
    };
    
    class OverloadedFunctionType : public Type, public Ides::Util::Singleton<OverloadedFunctionType> {
    public:
        OverloadedFunctionType() : Type("overloaded fn()", VoidType::GetSingletonPtr()) { }
    };
    
    class PointerType : public Type {
        PointerType(const Ides::Types::Type* targetType) : Type(targetType->ToString() + "*", VoidType::GetSingletonPtr()), targetType(targetType) { }
    public:
        typedef boost::unordered_map<const Ides::Types::Type*, PointerType*> PointerTypeMap;
        static const PointerType* Get(const Ides::Types::Type* target);
        
        virtual llvm::Type* GetLLVMType(ParseContext& ctx) const {
            if (targetType->IsEquivalentType(VoidType::GetSingletonPtr()))
                return llvm::PointerType::getUnqual(llvm::IntegerType::getInt8Ty(ctx.GetContext()));
            return llvm::PointerType::get(targetType->GetLLVMType(ctx), 0);
        }
        
        const Ides::Types::Type* GetTargetType() const { return this->targetType; }
    private:
        const Ides::Types::Type* targetType;
        static PointerTypeMap types;
    };
    
    class StructType : public Type {
    public:
        StructType(Ides::StringRef name) :
            Type("struct " + name, VoidType::GetSingletonPtr()), name(name) { }
        virtual ~StructType() { }
        
        static StructType* GetOrCreate(ParseContext& ctx, Ides::StringRef name);
        
        virtual llvm::Type* GetLLVMType(ParseContext& ctx) const { return type; }
        const Ides::Types::Type* GetMemberType(Ides::StringRef str) const;
        int GetMemberIndex(Ides::StringRef str) const;
        
        void SetMembers(ParseContext& ctx, const std::vector<std::pair<Ides::String, const Type*> >& members);
    private:
        llvm::StructType* type;
        Ides::String name;
        std::vector<std::pair<Ides::String, const Type*> > type_members;
        static llvm::StringMap<StructType*> types;
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
        friend class VoidType;
    public:
        typedef boost::function<llvm::Value*(ParseContext&, Ides::AST::AST*, Ides::AST::AST*)> GetValue;
        typedef boost::function<const Ides::Types::Type*(ParseContext&, Ides::AST::AST*, Ides::AST::AST*)> GetType;
        typedef std::pair<GetType, GetValue> NumericOperator;
        
        NumberType(Ides::StringRef type_name, Type* supertype) : Type(type_name, supertype) { }
        virtual ~NumberType() { }
        
        enum NumberClass {
            N_UINT,
            N_SINT,
            N_FLOAT
        };
        
        virtual NumberClass GetNumberClass() const = 0;
        virtual uint8_t GetSize() const = 0;
        
        bool IsSigned() const { return this->GetNumberClass() == N_SINT; }
    private:
        boost::unordered_map<Ides::String, NumericOperator> operators;
    };
    
#define IntegerType(size) \
    class Integer##size##Type : public NumberType, public Ides::Util::Singleton<Integer##size##Type> { \
    public: \
        Integer##size##Type() : NumberType("int" #size, VoidType::GetSingletonPtr()) { } \
        ~Integer##size##Type() { } \
        virtual llvm::Type* GetLLVMType(ParseContext& ctx) const { \
            return llvm::Type::getInt##size##Ty(ctx.GetContext()); \
        } \
        virtual NumberType::NumberClass GetNumberClass() const { return NumberType::N_SINT; } \
        virtual uint8_t GetSize() const { return size; } \
        virtual bool HasImplicitConversionTo(const Type* other) const; \
    }

#define UIntegerType(size) \
    class UInteger##size##Type : public NumberType, public Ides::Util::Singleton<UInteger##size##Type> { \
    public: \
        UInteger##size##Type() : NumberType("uint" #size, VoidType::GetSingletonPtr()) { } \
        ~UInteger##size##Type() { } \
        virtual llvm::Type* GetLLVMType(ParseContext& ctx) const { \
            return llvm::Type::getInt##size##Ty(ctx.GetContext()); \
        } \
        virtual NumberType::NumberClass GetNumberClass() const { return NumberType::N_UINT; } \
        virtual uint8_t GetSize() const { return size; } \
        virtual bool HasImplicitConversionTo(const Type* other) const; \
    }
    
    IntegerType(1);
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
            return llvm::Type::getFloatTy(ctx.GetContext());
        }
        virtual NumberType::NumberClass GetNumberClass() const { return NumberType::N_FLOAT; }
        virtual uint8_t GetSize() const { return 32; }
        virtual bool HasImplicitConversionTo(const Type* other) const;
    };
    
    class Float64Type : public NumberType, public Ides::Util::Singleton<Float64Type> {
    public:
        Float64Type() : NumberType("float64", VoidType::GetSingletonPtr()) { }
        virtual ~Float64Type() { }
        virtual llvm::Type* GetLLVMType(ParseContext& ctx) const {
            return llvm::Type::getDoubleTy(ctx.GetContext());
        }
        virtual NumberType::NumberClass GetNumberClass() const { return NumberType::N_FLOAT; }
        virtual uint8_t GetSize() const { return 64; }
        virtual bool HasImplicitConversionTo(const Type* other) const;
    };
    
} // namespace Types
} // namespace Ides
#endif // _IDES_TYPES_H_

