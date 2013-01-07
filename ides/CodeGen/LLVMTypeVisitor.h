//
//  LLVMTypeVisitor.h
//  ides
//
//  Created by Sean Edwards on 1/4/13.
//
//

#ifndef __ides__LLVMTypeVisitor__
#define __ides__LLVMTypeVisitor__

#include <ides/common.h>
#include <ides/Types/Type.h>
#include <ides/Types/TypeVisitor.h>

namespace Ides {
namespace CodeGen {
    
    class LLVMTypeVisitor : public Ides::Types::TypeVisitor {
    public:
        LLVMTypeVisitor(llvm::LLVMContext& ctx) : t(NULL), ctx(ctx) { }
        
        virtual void Visit(const Ides::Types::VoidType* ty) { t = llvm::Type::getVoidTy(ctx); }
        virtual void Visit(const Ides::Types::UnitType* ty) { t = llvm::Type::getVoidTy(ctx); }
        
        virtual void Visit(const Ides::Types::PointerType* ty) {
            if (ty->GetTargetType()->IsEquivalentType(Ides::Types::VoidType::GetSingletonPtr()))
                t = llvm::PointerType::getUnqual(llvm::IntegerType::getInt8Ty(ctx));
            else {
                ty->GetTargetType()->Accept(this);
                t = llvm::PointerType::getUnqual(t);
            }
        }
        
        virtual void Visit(const Ides::Types::FunctionType* ty);
        virtual void Visit(const Ides::Types::OverloadedFunctionType* ty) { t = NULL; }
        
        virtual void Visit(const Ides::Types::StructType* ty);
        virtual void Visit(const Ides::Types::ClassType* ty) { }
        virtual void Visit(const Ides::Types::ReferenceType* ty) {  }
        
        virtual void Visit(const Ides::Types::Integer1Type* ty) { t = llvm::Type::getInt1Ty(ctx); }
        virtual void Visit(const Ides::Types::Integer8Type* ty) { t = llvm::Type::getInt8Ty(ctx); }
        virtual void Visit(const Ides::Types::UInteger8Type* ty) { t = llvm::Type::getInt8Ty(ctx); }
        virtual void Visit(const Ides::Types::Integer16Type* ty) { t = llvm::Type::getInt16Ty(ctx); }
        virtual void Visit(const Ides::Types::UInteger16Type* ty) { t = llvm::Type::getInt16Ty(ctx); }
        virtual void Visit(const Ides::Types::Integer32Type* ty) { t = llvm::Type::getInt32Ty(ctx); }
        virtual void Visit(const Ides::Types::UInteger32Type* ty) { t = llvm::Type::getInt32Ty(ctx); }
        virtual void Visit(const Ides::Types::Integer64Type* ty) { t = llvm::Type::getInt64Ty(ctx); }
        virtual void Visit(const Ides::Types::UInteger64Type* ty) { t = llvm::Type::getInt64Ty(ctx); }
        
        virtual void Visit(const Ides::Types::Float32Type* ty) { t = llvm::Type::getFloatTy(ctx); }
        virtual void Visit(const Ides::Types::Float64Type* ty) { t = llvm::Type::getDoubleTy(ctx); }
        
        virtual void Visit(const Ides::Types::IntegerLiteralType<8>* ty) { t = llvm::Type::getInt8Ty(ctx); }
        virtual void Visit(const Ides::Types::IntegerLiteralType<16>* ty) { t = llvm::Type::getInt16Ty(ctx); }
        virtual void Visit(const Ides::Types::IntegerLiteralType<32>* ty) { t = llvm::Type::getInt32Ty(ctx); }
        virtual void Visit(const Ides::Types::IntegerLiteralType<64>* ty) { t = llvm::Type::getInt64Ty(ctx); }
        
        
        llvm::Type* GetType() { return t; }
    private:
        llvm::Type* t;
        llvm::LLVMContext& ctx;
        
        boost::unordered_map<Ides::String, llvm::StructType*> structTypes;
    };
    
}
}

#endif /* defined(__ides__LLVMTypeVisitor__) */
