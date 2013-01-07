//
//  CodeGen.cpp
//  ides
//
//  Created by Sean Edwards on 12/30/12.
//
//

#include "CodeGen.h"
#include <ides/Diagnostics/Diagnostics.h>

#include <ides/AST/ConstantExpression.h>

#include <ides/CodeGen/LLVMTypeVisitor.h>
#include "llvm/Analysis/Verifier.h"


namespace Ides {
namespace CodeGen {
    
    using namespace Ides::Diagnostics;
    
    CodeGen::CodeGen(clang::IntrusiveRefCntPtr<clang::DiagnosticsEngine> diags, llvm::LLVMContext& lctx, Ides::AST::ASTContext& actx)
        : lctx(lctx), typeVisitor(lctx), actx(actx), diag(diags)
    {
        this->module = new llvm::Module("Ides Module", lctx);
        this->builder = new llvm::IRBuilder<>(lctx);
    }
    
    CodeGen::~CodeGen() {
        delete builder;
        //delete module;
    }
    
    void CodeGen::Compile(Ides::AST::CompilationUnit* ast) { SETTRACE("CodeGen::Compile")
        try {
            ast->Accept(this);
            if(!llvm::verifyModule(*this->module, llvm::PrintMessageAction))
                this->module->dump();
        }
        catch (const std::exception&) {
        }
        functions.clear();
    }
    
    llvm::Value* CodeGen::GetValue(Ides::AST::Expression* ast) { SETTRACE("CodeGen::GetValue")
        ast->Accept(this);
        const Ides::Types::Type* exprType = ast->GetType(actx);
        llvm::Type* valType = last->getType();
        while (exprType->IsPtrType()) {
            assert(valType->isPointerTy());
            
            exprType = static_cast<const Ides::Types::PointerType*>(exprType)->GetTargetType();
            valType = llvm::dyn_cast<llvm::PointerType>(valType)->getElementType();
        }
        if (valType->isPointerTy())
            last = builder->CreateLoad(last, "autoderef");
        return last;
    }
    
    llvm::Value* CodeGen::GetPtr(Ides::AST::Expression* ast) { SETTRACE("CodeGen::GetPtr")
        ast->Accept(this);
        const Ides::Types::Type* exprType = ast->GetType(actx);
        llvm::Type* valType = last->getType();
        while (exprType->IsPtrType()) {
            exprType = static_cast<const Ides::Types::PointerType*>(exprType)->GetTargetType();
            valType = llvm::dyn_cast<llvm::PointerType>(valType);
        }
        
        if (valType->isPointerTy()) return last;
        
        throw detail::CodeGenError(*diag, INVALID_TEMPORARY_VALUE, ast->exprloc);
    }
    
    llvm::Value* CodeGen::GetValue(Ides::AST::Expression* ast, const Ides::Types::Type* toType) {
        const Ides::Types::Type* fromType = ast->GetType(actx);
        if (fromType->IsEquivalentType(toType)) {
            return GetValue(ast);
        }
        else if (fromType->HasImplicitConversionTo(toType)) {
            // We've determined that an implicit conversion is safe, so go through the normal cast codepath.
            return Cast(ast, toType);
        }
        throw detail::CodeGenError(*diag, NO_IMPLICIT_CONVERSION, ast->exprloc) << fromType->ToString() << toType->ToString();
    }
    
    
    llvm::Value* CodeGen::Cast(Ides::AST::Expression* ast, const Ides::Types::Type* toType) {
        const Ides::Types::Type* exprtype = ast->GetType(actx);
        if (exprtype->IsEquivalentType(toType)) return GetValue(ast);
        
        if (exprtype->IsNumericType()) {
            const Ides::Types::NumberType* exprnumtype = static_cast<const Ides::Types::NumberType*>(exprtype);
            if (toType->IsNumericType()) {
                const Ides::Types::NumberType* tonumtype = static_cast<const Ides::Types::NumberType*>(toType);
                
                switch (exprnumtype->GetNumberClass()) {
                    case Ides::Types::NumberType::N_UINT:
                        if (tonumtype->GetNumberClass() == Ides::Types::NumberType::N_FLOAT) {
                            return builder->CreateUIToFP(GetValue(ast), GetLLVMType(tonumtype));
                        }
                    case Ides::Types::NumberType::N_SINT:
                        if (tonumtype->GetNumberClass() == Ides::Types::NumberType::N_FLOAT) {
                            return builder->CreateSIToFP(GetValue(ast), GetLLVMType(tonumtype));
                        }
                        
                        return builder->CreateIntCast(GetValue(ast), GetLLVMType(tonumtype),
                                                      exprnumtype->GetNumberClass() == Ides::Types::NumberType::N_SINT);
                        
                    case Ides::Types::NumberType::N_FLOAT:
                        if (tonumtype->GetNumberClass() == Ides::Types::NumberType::N_FLOAT) {
                            return builder->CreateFPCast(GetValue(ast), GetLLVMType(tonumtype));
                        }
                        else if (tonumtype->IsSigned()) {
                            return builder->CreateFPToSI(GetValue(ast), GetLLVMType(tonumtype));
                        }
                        else {
                            return builder->CreateFPToUI(GetValue(ast), GetLLVMType(tonumtype));
                        }
                }
            }
            else if (toType->IsPtrType()) {
                return builder->CreateIntToPtr(GetValue(ast), GetLLVMType(toType));
            }
        }
        else if (exprtype->IsPtrType()) {
            if (toType->IsIntegerType()) {
                return builder->CreatePtrToInt(GetValue(ast), GetLLVMType(toType));
            }
        }
        throw detail::CodeGenError(*diag, NO_EXPLICIT_CAST, ast->exprloc) << exprtype->ToString() << toType->ToString();
    }
    
    void CodeGen::Visit(Ides::AST::CompilationUnit* ast) {
        Ides::AST::ASTContext::DeclScope typescope(actx, ast);
        
        for (auto i = ast->begin(); i != ast->end(); ++i) {
            i->second->Accept(this);
        }
        
        auto initializerStruct = llvm::StructType::get(llvm::Type::getInt32Ty(lctx), llvm::FunctionType::get(llvm::Type::getVoidTy(lctx), false)->getPointerTo(), NULL);
        auto initializerArray = llvm::ArrayType::get(initializerStruct, this->globalInitializers.size());
        
        std::vector<llvm::Constant*> initializers;
        for (auto i = globalInitializers.begin(); i != globalInitializers.end(); ++i) {
            initializers.push_back(llvm::ConstantStruct::get(initializerStruct, llvm::ConstantInt::get(llvm::Type::getInt32Ty(lctx), i->first), i->second, NULL));
        }
        auto constructorArray = llvm::ConstantArray::get(initializerArray, initializers);
        
        auto constructorArrayVar = new llvm::GlobalVariable(*module, initializerArray, false, llvm::GlobalValue::AppendingLinkage, constructorArray, "llvm.global_ctors");
        
        
    }
    
    llvm::Type* CodeGen::GetLLVMType(const Ides::Types::Type* ty) {
        ty->Accept(&typeVisitor);
        return typeVisitor.GetType();
    }
    
    
}
}