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
    
    class Integer1Type;
    class Integer8Type;
    class UInteger8Type;
    class Integer16Type;
    class UInteger16Type;
    class Integer32Type;
    class UInteger32Type;
    class Integer64Type;
    class UInteger64Type;
    
    class Float32Type;
    class Float64Type;
    
    template<uint8_t size>
    class IntegerLiteralType;
    
    class FloatLiteralType;
    
    class TypeVisitor {
    public:
        virtual void Visit(const Ides::Types::VoidType* ty) = 0;
        virtual void Visit(const Ides::Types::UnitType* ty) = 0;
        
        virtual void Visit(const Ides::Types::PointerType* ty) = 0;
        virtual void Visit(const Ides::Types::FunctionType* ty) = 0;
        virtual void Visit(const Ides::Types::OverloadedFunctionType* ty) = 0;
        
        virtual void Visit(const Ides::Types::StructType* ty) = 0;
        virtual void Visit(const Ides::Types::ClassType* ty) = 0;
        virtual void Visit(const Ides::Types::ReferenceType* ty) = 0;
        
        virtual void Visit(const Ides::Types::Integer1Type* ty) = 0;
        virtual void Visit(const Ides::Types::Integer8Type* ty) = 0;
        virtual void Visit(const Ides::Types::UInteger8Type* ty) = 0;
        virtual void Visit(const Ides::Types::Integer16Type* ty) = 0;
        virtual void Visit(const Ides::Types::UInteger16Type* ty) = 0;
        virtual void Visit(const Ides::Types::Integer32Type* ty) = 0;
        virtual void Visit(const Ides::Types::UInteger32Type* ty) = 0;
        virtual void Visit(const Ides::Types::Integer64Type* ty) = 0;
        virtual void Visit(const Ides::Types::UInteger64Type* ty) = 0;
        
        virtual void Visit(const Ides::Types::Float32Type* ty) = 0;
        virtual void Visit(const Ides::Types::Float64Type* ty) = 0;
        
        virtual void Visit(const Ides::Types::IntegerLiteralType<8>* ty) = 0;
        virtual void Visit(const Ides::Types::IntegerLiteralType<16>* ty) = 0;
        virtual void Visit(const Ides::Types::IntegerLiteralType<32>* ty) = 0;
        virtual void Visit(const Ides::Types::IntegerLiteralType<64>* ty) = 0;
        virtual void Visit(const Ides::Types::FloatLiteralType* ty) = 0;
        
    };
    
}
}

#endif /* defined(__ides__TypeVisitor__) */
