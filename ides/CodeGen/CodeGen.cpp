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
        } catch (const detail::CodeGenError&) {
            std::cerr << "Build completed with errors." << std::endl;
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
        
        Diag(INVALID_TEMPORARY_VALUE, ast);
        throw detail::CodeGenError();
    }
    
    llvm::Value* CodeGen::GetValue(Ides::AST::Expression* ast, const Ides::Types::Type* toType) {
        llvm::Value* val = GetValue(ast);
        const Ides::Types::Type* fromType = ast->GetType(actx);
        if (fromType->IsEquivalentType(toType)) {
            // The types are identical. Do nothing.
        }
        else if (fromType->HasImplicitConversionTo(toType)) {
            // All implicit conversions are defined as conversions that do not involve
            // losing or rewriting data. A blind bitcast is sufficient.
            val = builder->CreateBitCast(val, this->GetLLVMType(toType));
        } else {
            Diag(NO_IMPLICIT_CONVERSION, ast) << fromType->ToString() << toType->ToString();
            throw detail::CodeGenError();
        }
        return val;
    }
    
    llvm::Value* CodeGen::GetPtr(Ides::AST::Expression* ast, const Ides::Types::Type* toType) {
        llvm::Value* ptr = GetPtr(ast);
        const Ides::Types::Type* fromType = ast->GetType(actx);
        if (fromType->IsEquivalentType(toType)) {
            // The types are identical. Do nothing.
        }
        else if (fromType->HasImplicitConversionTo(toType)) {
            // All implicit conversions are defined as conversions that do not involve
            // losing or rewriting data. A blind bitcast is sufficient.
            ptr = builder->CreateBitCast(ptr, this->GetLLVMType(toType));
        } else {
            Diag(NO_IMPLICIT_CONVERSION, ast) << fromType->ToString() << toType->ToString();
            throw detail::CodeGenError();
        }
        return ptr;
    }
    
    void CodeGen::Visit(Ides::AST::CompilationUnit* ast) {
        Ides::AST::ASTContext::DeclScope typescope(actx, ast);
        
        for (auto i = ast->begin(); i != ast->end(); ++i) {
            i->second->Accept(this);
        }
    }
    
    llvm::Type* CodeGen::GetLLVMType(const Ides::Types::Type* ty) {
        ty->Accept(&typeVisitor);
        return typeVisitor.GetType();
    }
    
    
}
}