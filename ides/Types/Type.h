#ifndef _IDES_TYPES_H_
#define _IDES_TYPES_H_

#include <ides/common.h>
#include <ides/AST/ASTContext.h>
#include <boost/function.hpp>
#include <boost/unordered_map.hpp>
#include <ides/AST/DeclarationContext.h>
#include <ides/Types/TypeVisitor.h>

#include <boost/lexical_cast.hpp>

namespace Ides {
    namespace AST {
        class AST;
        class Declaration;
    }
namespace Types {

    typedef Ides::AST::ASTContext ParseContext;
    
    class PointerType;
    
    class Type {
    public:
        Type(Ides::StringRef type_name, Type* supertype) :
            type_name(type_name), supertype(supertype),
            static_members(supertype ? supertype->static_members : NULL),
            instance_members(supertype ? supertype->instance_members : NULL)
        {
            typenames.insert(std::make_pair(type_name, this));
        }
        virtual void Accept(Ides::Types::TypeVisitor* visitor) const = 0;
        
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
        
        virtual bool IsPtrType() const { return false; }
        virtual bool IsNumericType() const { return IsIntegerType() || IsFloatType(); }
        virtual bool IsIntegerType() const { return false; }
        virtual bool IsFloatType() const { return false; }
        
        virtual unsigned int GetAlignment() const { return 1; }
        virtual uint64_t GetSize() const = 0;
        
        
        virtual const Ides::String ToString() const { return type_name; }
        
        Ides::AST::Declaration* GetStaticMember(Ides::AST::ASTContext& ctx, Ides::StringRef name) const {
            return static_members.GetMember(ctx, name);
        }
        
        Ides::AST::Declaration* GetInstanceMember(Ides::AST::ASTContext& ctx, Ides::StringRef name) const {
            return instance_members.GetMember(ctx, name);
        }
        
        void AddStaticMember(Ides::StringRef name, Ides::AST::Declaration* ast) {
            static_members.AddMember(name, ast);
        }
        
        void AddInstanceMember(Ides::StringRef name, Ides::AST::Declaration* ast) {
            instance_members.AddMember(name, ast);
        }
    protected:
        Ides::String type_name;
        const Type* supertype;
        
        static boost::unordered_map<Ides::String, const Ides::Types::Type*> typenames;
        
        Ides::AST::HierarchicalConcreteDeclarationContext static_members;
        Ides::AST::HierarchicalConcreteDeclarationContext instance_members;
    };
    
    class VoidType : public Type, public Ides::Util::Singleton<VoidType> {
    public:
        VoidType() : Type("void", NULL) { }
        virtual void Accept(Ides::Types::TypeVisitor* visitor) const { visitor->Visit(this); }
        
        virtual bool IsSupertypeOf(const Type* other) const {
            return true; // VoidType is a supertype of all other types.
        }
        virtual bool IsSubtypeOf(const Type* other) const {
            return false; // VoidType is the universal subtype
        }
        
        virtual uint64_t GetSize() const { return 0; }
    };
    
    class UnitType : public Type, public Ides::Util::Singleton<UnitType> {
    public:
        UnitType() : Type("unit", NULL) { }
        virtual void Accept(Ides::Types::TypeVisitor* visitor) const { visitor->Visit(this); }
        
        virtual bool IsSupertypeOf(const Type* other) const {
            return false; // Nothing inherits from Unit.
        }
        
        virtual bool IsSubtypeOf(const Type* other) const {
            return true; // Unit is a subtype of all other types.
        }
        
        virtual uint64_t GetSize() const { return 0; }
        
    };
    
    class FunctionType : public Type {
        FunctionType(const Ides::Types::Type* retType, const std::vector<const Ides::Types::Type*>& argTypes, bool varArgs = false) :
            Type("", VoidType::GetSingletonPtr()), retType(retType), argTypes(argTypes), isVarArgs(varArgs)
        {
            std::stringstream t;
            t << "fn(";
            
            auto i = argTypes.begin();
            if (i != argTypes.end()) { t << (*i)->ToString(); ++i; }
            for (; i != argTypes.end(); ++i) {
                t << ", " << (*i)->ToString();
            }
            if (varArgs) t << "...";
            
            t << ") : " << retType->ToString();
            this->type_name = t.str();
        }
    public:
        virtual void Accept(Ides::Types::TypeVisitor* visitor) const { visitor->Visit(this); }
        
        typedef boost::unordered_set<FunctionType*> FunctionTypeSet;
        static const FunctionType* Get(const Ides::Types::Type* retType, const std::vector<const Ides::Types::Type*>& argTypes, bool isVarArgs = false);
        
        virtual uint64_t GetSize() const { return 64; }
        
        virtual const Ides::String ToString() const;

        const Ides::Types::Type* retType;
        const std::vector<const Ides::Types::Type*> argTypes;
        const bool isVarArgs;
        
    private:
        static FunctionTypeSet types;
    };
    
    class OverloadedFunctionType : public Type, public Ides::Util::Singleton<OverloadedFunctionType> {
    public:
        OverloadedFunctionType() : Type("overloaded fn()", VoidType::GetSingletonPtr()) { }
        virtual void Accept(Ides::Types::TypeVisitor* visitor) const { visitor->Visit(this); }
        
        virtual uint64_t GetSize() const { return 0; }
        
    };
    
    class PointerType : public Type {
        PointerType(const Ides::Types::Type* targetType) : Type(targetType->ToString() + "*", VoidType::GetSingletonPtr()), targetType(targetType) { }
    public:
        virtual void Accept(Ides::Types::TypeVisitor* visitor) const { visitor->Visit(this); }
        
        typedef boost::unordered_map<const Ides::Types::Type*, PointerType*> PointerTypeMap;
        static const PointerType* Get(const Ides::Types::Type* target);
        
        const Ides::Types::Type* GetTargetType() const { return this->targetType; }
        
        virtual bool IsPtrType() const { return true; }
        virtual uint64_t GetSize() const { return 64; }
        virtual unsigned int GetAlignment() const { return GetSize(); }
    private:
        const Ides::Types::Type* targetType;
        static PointerTypeMap types;
    };
    
    class StructType : public Type {
    public:
        StructType(Ides::StringRef name) :
            Type("struct " + name, VoidType::GetSingletonPtr()), name(name) { }
        virtual ~StructType() { }
        virtual void Accept(Ides::Types::TypeVisitor* visitor) const { visitor->Visit(this); }
        
        Ides::StringRef GetName() const { return name; }
        
        static StructType* GetOrCreate(ParseContext& ctx, Ides::StringRef name);
        
        const Ides::Types::Type* GetMemberType(Ides::StringRef str) const;
        int GetMemberIndex(Ides::StringRef str) const;
        
        virtual uint64_t GetSize() const {
            uint64_t size = 0;
            for (auto i = type_members.begin(); i != type_members.end(); ++i) {
                size += i->second->GetSize();
            }
            return size;
        }
        
        void SetMembers(ParseContext& ctx, const std::vector<std::pair<Ides::String, const Type*> >& members);
        const std::vector<std::pair<Ides::String, const Type*> >& GetMembers() const { return type_members; }
    private:
        llvm::StructType* type;
        Ides::String name;
        std::vector<std::pair<Ides::String, const Type*> > type_members;
        static boost::unordered_map<Ides::String, StructType*> types;
    };
    
    
    class ClassType : public Type {
    public:
        virtual void Accept(Ides::Types::TypeVisitor* visitor) const { visitor->Visit(this); }
        
    };
    
    class ReferenceType : public Type {
    public:
        virtual void Accept(Ides::Types::TypeVisitor* visitor) const { visitor->Visit(this); }
        
    };
    
    class StringType : public ReferenceType {
    public:
        virtual void Accept(Ides::Types::TypeVisitor* visitor) const { visitor->Visit(this); }
        
    };

    class Integer1Type : public Type, public Ides::Util::Singleton<Integer1Type> {
    public:
        Integer1Type() : Type("bool", VoidType::GetSingletonPtr()) { }

        virtual void Accept(Ides::Types::TypeVisitor* visitor) const { visitor->Visit(this); }

        virtual bool HasImplicitConversionTo(const Type* other) const {
            return false;
        }

        virtual uint64_t GetSize() const { return 1; }
    };
    
    class NumberType : public Type {
        friend class VoidType;
    public:
        typedef boost::function<llvm::Value*(ParseContext&, Ides::AST::AST*, Ides::AST::AST*)> GetValue;
        typedef boost::function<const Ides::Types::Type*(ParseContext&, Ides::AST::AST*, Ides::AST::AST*)> GetType;
        typedef std::pair<GetType, GetValue> NumericOperator;
        
        NumberType(Ides::StringRef type_name, Type* supertype) : Type(type_name, supertype) { }
        
        enum NumberClass {
            N_UINT,
            N_SINT,
            N_FLOAT
        };
        
        virtual NumberClass GetNumberClass() const = 0;
        
        bool IsSigned() const { return this->GetNumberClass() == N_SINT; }
        virtual unsigned int GetAlignment() const { return GetSize(); }
    private:
        boost::unordered_map<Ides::String, NumericOperator> operators;
    };
    
    template<uint8_t size>
    class IntegerLiteralType : public NumberType, public Ides::Util::Singleton<IntegerLiteralType<size> > {
    public:
        IntegerLiteralType() : NumberType(Ides::String("int") + boost::lexical_cast<Ides::String>((int32_t)size), VoidType::GetSingletonPtr()) { }
        
        virtual void Accept(Ides::Types::TypeVisitor* visitor) const { visitor->Visit(this); }
        
        virtual NumberClass GetNumberClass() const { return NumberType::N_SINT; }
        virtual uint64_t GetSize() const { return size; }
        virtual bool IsIntegerType() const { return true; }
        virtual bool IsSigned() const { return false; }
        virtual bool HasImplicitConversionTo(const Type* other) const {
            if (auto nt = dynamic_cast<const NumberType*>(other)) {
                return nt->GetSize() >= size || (nt->IsSigned() == false && nt->GetSize() * 2 >= size);
            }
            return false;
        }
    };
    
#define IntegerType(size) \
    class Integer##size##Type : public NumberType, public Ides::Util::Singleton<Integer##size##Type> { \
    public: \
        virtual void Accept(Ides::Types::TypeVisitor* visitor) const { visitor->Visit(this); } \
        Integer##size##Type() : NumberType("int" #size, VoidType::GetSingletonPtr()) { } \
        ~Integer##size##Type() { } \
        virtual NumberType::NumberClass GetNumberClass() const { return NumberType::N_SINT; } \
        virtual uint64_t GetSize() const { return size; } \
        virtual bool HasImplicitConversionTo(const Type* other) const; \
        virtual bool IsIntegerType() const { return true; } \
    }

#define UIntegerType(size) \
    class UInteger##size##Type : public NumberType, public Ides::Util::Singleton<UInteger##size##Type> { \
    public: \
        virtual void Accept(Ides::Types::TypeVisitor* visitor) const { visitor->Visit(this); } \
        UInteger##size##Type() : NumberType("uint" #size, VoidType::GetSingletonPtr()) { } \
        ~UInteger##size##Type() { } \
        virtual NumberType::NumberClass GetNumberClass() const { return NumberType::N_UINT; } \
        virtual uint64_t GetSize() const { return size; } \
        virtual bool HasImplicitConversionTo(const Type* other) const; \
        virtual bool IsIntegerType() const { return true; } \
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
        virtual void Accept(Ides::Types::TypeVisitor* visitor) const { visitor->Visit(this); }
        
        virtual NumberType::NumberClass GetNumberClass() const { return NumberType::N_FLOAT; }
        virtual uint64_t GetSize() const { return 32; }
        virtual bool HasImplicitConversionTo(const Type* other) const;
        virtual bool IsFloatType() const { return true; }
    };
    
    class Float64Type : public NumberType, public Ides::Util::Singleton<Float64Type> {
    public:
        Float64Type() : NumberType("float64", VoidType::GetSingletonPtr()) { }
        virtual ~Float64Type() { }
        virtual void Accept(Ides::Types::TypeVisitor* visitor) const { visitor->Visit(this); }
        
        virtual NumberType::NumberClass GetNumberClass() const { return NumberType::N_FLOAT; }
        virtual uint64_t GetSize() const { return 64; }
        virtual bool HasImplicitConversionTo(const Type* other) const;
        virtual bool IsFloatType() const { return true; }
    };
    
} // namespace Types
} // namespace Ides
#endif // _IDES_TYPES_H_

