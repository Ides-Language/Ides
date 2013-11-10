//
//  TypeVisitor.h
//  ides
//
//  Created by Sean Edwards on 1/4/13.
//
//

#ifndef __ides__TypeVisitor__
#define __ides__TypeVisitor__
#include <stdint.h>

namespace Ides {
namespace Types {
    
    class VoidType;
    class UnitType;
    
    class PointerType;
    class FunctionType;
    class OverloadedFunctionType;
    
    class StructType;
    class ClassType;
    class ReferenceType;
    
    class Float32Type;
    class Float64Type;
    
    template<uint8_t size>
    class IntegerLiteralType;

    class Integer1Type;
    
    template<uint64_t size, bool isSigned>
    class IntegerType;
    
    typedef IntegerType<8, true> Integer8Type;
    typedef IntegerType<16, true> Integer16Type;
    typedef IntegerType<32, true> Integer32Type;
    typedef IntegerType<64, true> Integer64Type;

    typedef IntegerType<8, false> UInteger8Type;
    typedef IntegerType<16, false> UInteger16Type;
    typedef IntegerType<32, false> UInteger32Type;
    typedef IntegerType<64, false> UInteger64Type;
    
    class FloatLiteralType;
    
    class TypeVisitor {
    public:
        virtual void Visit(const Ides::Types::VoidType* ty) { }
        virtual void Visit(const Ides::Types::UnitType* ty) { }
        
        virtual void Visit(const Ides::Types::PointerType* ty) { }
        virtual void Visit(const Ides::Types::FunctionType* ty) { }
        virtual void Visit(const Ides::Types::OverloadedFunctionType* ty) { }
        
        virtual void Visit(const Ides::Types::StructType* ty) { }
        virtual void Visit(const Ides::Types::ClassType* ty) { }
        virtual void Visit(const Ides::Types::ReferenceType* ty) { }
        
        virtual void Visit(const Ides::Types::Integer1Type* ty) { }
        virtual void Visit(const Ides::Types::Integer8Type* ty) { }
        virtual void Visit(const Ides::Types::UInteger8Type* ty) { }
        virtual void Visit(const Ides::Types::Integer16Type* ty) { }
        virtual void Visit(const Ides::Types::UInteger16Type* ty) { }
        virtual void Visit(const Ides::Types::Integer32Type* ty) { }
        virtual void Visit(const Ides::Types::UInteger32Type* ty) { }
        virtual void Visit(const Ides::Types::Integer64Type* ty) { }
        virtual void Visit(const Ides::Types::UInteger64Type* ty) { }
        
        virtual void Visit(const Ides::Types::Float32Type* ty) { }
        virtual void Visit(const Ides::Types::Float64Type* ty) { }
        
        virtual void Visit(const Ides::Types::IntegerLiteralType<8>* ty) { }
        virtual void Visit(const Ides::Types::IntegerLiteralType<16>* ty) { }
        virtual void Visit(const Ides::Types::IntegerLiteralType<32>* ty) { }
        virtual void Visit(const Ides::Types::IntegerLiteralType<64>* ty) { }
        
    };
    
}
}

#endif /* defined(__ides__TypeVisitor__) */
