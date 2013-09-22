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
#include <ides/ASTSerializer/MetadataSerializer.h>
#include "llvm/Analysis/Verifier.h"


namespace Ides {
namespace CodeGen {
    
    using namespace Ides::Diagnostics;
    
    CodeGen::CodeGen(llvm::IntrusiveRefCntPtr<clang::DiagnosticsEngine> diags,
                     llvm::LLVMContext& lctx,
                     Ides::AST::ASTContext& actx,
                     clang::FileManager* fman,
                     clang::SourceManager* sman)
    : fman(fman), sman(sman), lctx(lctx), actx(actx), typeVisitor(lctx), last(NULL), diag(diags)
    {
        this->staticInitializerSequence = 0;
        this->module = new llvm::Module("Ides Module", lctx);
        this->builder = new llvm::IRBuilder<>(lctx);
        this->dibuilder = new DIGenerator(*this->module);
    }
    
    CodeGen::~CodeGen() {
        if (dibuilder) delete dibuilder;
        delete builder;
        //delete module;
    }
    
    void CodeGen::Compile(Ides::AST::CompilationUnit* ast) { SETTRACE("CodeGen::Compile")
        try {
            this->module->setModuleIdentifier(sman->getFileEntryForID(ast->GetFile())->getName());
            ast->Accept(this);
            if (dibuilder) dibuilder->finalize();
            llvm::verifyModule(*this->module);
            values.clear();
        }
        catch (const std::exception& ex) {
            values.clear();
            throw ex;
        }
    }
    
    llvm::Value* CodeGen::GetValue(Ides::AST::Expression* ast) { SETTRACE("CodeGen::GetValue")
        DeclarationGuard _guard(this->isDeclaration, false);
        
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
        
        EmitDebugLoc(ast);
        return last;
    }
    
    llvm::Value* CodeGen::GetValue(Ides::AST::Expression* ast, const Ides::Types::Type* toType) {
        DeclarationGuard _guard(this->isDeclaration, false);
        
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
    
    llvm::Value* CodeGen::GetPtr(Ides::AST::Expression* ast) { SETTRACE("CodeGen::GetPtr")
        DeclarationGuard _guard(this->isDeclaration, false);
        
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
    
    llvm::Value* CodeGen::GetDecl(Ides::AST::Declaration* ast) {
        DeclarationGuard _guard(this->isDeclaration, true);
        ast->Accept(this);
        EmitDebugLoc(ast);
        return last;
    }
    
    void CodeGen::EmitDebugLoc(Ides::AST::AST* ast) {
        if (last != NULL && llvm::isa<llvm::Instruction>(last) && dibuilder != NULL) {
            llvm::cast<llvm::Instruction>(last)->setDebugLoc(this->GetDebugLoc(ast));
        }
    }
    
    
    llvm::DebugLoc CodeGen::GetDebugLoc(Ides::AST::AST* ast) {
        assert(dibuilder != NULL);
        auto offset = sman->getFileOffset(ast->exprloc.getBegin());
        return llvm::DebugLoc::get(sman->getLineNumber(sman->getMainFileID(), offset),
                                   sman->getColumnNumber(sman->getMainFileID(), offset),
                                   const_cast<llvm::MDNode*>(dibuilder->GetCurrentScope()),
                                   NULL);
    }
    
    
    llvm::Value* CodeGen::Cast(Ides::AST::Expression* ast, const Ides::Types::Type* toType) {
        DeclarationGuard _guard(this->isDeclaration, false);
        
        llvm::Value* ret = NULL;
        
        const Ides::Types::Type* exprtype = ast->GetType(actx);
        if (exprtype->IsEquivalentType(toType)) {
            ret = GetValue(ast);
        }
        else if (exprtype->IsNumericType()) {
            const Ides::Types::NumberType* exprnumtype = static_cast<const Ides::Types::NumberType*>(exprtype);
            if (toType->IsNumericType()) {
                const Ides::Types::NumberType* tonumtype = static_cast<const Ides::Types::NumberType*>(toType);
                
                switch (exprnumtype->GetNumberClass()) {
                    case Ides::Types::NumberType::N_UINT:
                        if (tonumtype->GetNumberClass() == Ides::Types::NumberType::N_FLOAT) {
                            ret = builder->CreateUIToFP(GetValue(ast), GetLLVMType(tonumtype));
                            break;
                        }
                    case Ides::Types::NumberType::N_SINT:
                        if (tonumtype->GetNumberClass() == Ides::Types::NumberType::N_FLOAT) {
                            ret = builder->CreateSIToFP(GetValue(ast), GetLLVMType(tonumtype));
                        }
                        else {
                            ret = builder->CreateIntCast(GetValue(ast), GetLLVMType(tonumtype),
                                                         exprnumtype->GetNumberClass() == Ides::Types::NumberType::N_SINT);
                        }
                        break;
                        
                    case Ides::Types::NumberType::N_FLOAT:
                        if (tonumtype->GetNumberClass() == Ides::Types::NumberType::N_FLOAT) {
                            ret = builder->CreateFPCast(GetValue(ast), GetLLVMType(tonumtype));
                        }
                        else if (tonumtype->IsSigned()) {
                            ret = builder->CreateFPToSI(GetValue(ast), GetLLVMType(tonumtype));
                        }
                        else {
                            ret = builder->CreateFPToUI(GetValue(ast), GetLLVMType(tonumtype));
                        }
                        break;
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
            else if (toType->IsPtrType()) {
                return builder->CreateBitCast(GetValue(ast), GetLLVMType(toType));
            }
        }
        
        if (ret == NULL) {
            throw detail::CodeGenError(*diag, NO_EXPLICIT_CAST, ast->exprloc) << exprtype->ToString() << toType->ToString();
        }
        
        EmitDebugLoc(ast);
        return ret;
    }
    
    void CodeGen::Visit(Ides::AST::CompilationUnit* ast) {
        Ides::AST::ASTContext::DeclScope typescope(actx, ast);
        
        DIGenerator::DebugScope dbgscope;
        this->fid = ast->GetFile();
        if (dibuilder) {
            const clang::FileEntry* fe = sman->getFileEntryForID(this->fid);
            dibuilder->createCompileUnit(llvm::dwarf::DW_LANG_C, fe->getName(), fe->getDir()->getName(), "idesc", false, "", 0);
            diFile = dibuilder->createFile(fe->getName(), fe->getDir()->getName());
            dbgscope.SetScope(dibuilder, diFile);
        }

        Ides::AST::MetadataSerializer md(this->actx, this->lctx);

        llvm::NamedMDNode* mdn = module->getOrInsertNamedMetadata("ides.link");

        bool errors = false;
        for (auto i = ast->begin(); i != ast->end(); ++i) {
            if (dynamic_cast<Ides::AST::StructDeclaration*>(i->second)) {
                this->GetDecl(i->second);
                llvm::MDNode* node = md.GetMDValue(i->second);
                if (node != NULL) mdn->addOperand(node);
            }
        }
        
        for (auto i = ast->begin(); i != ast->end(); ++i) {
            try {
                Ides::AST::Declaration* decl = i->second;
                if (!dynamic_cast<Ides::AST::StructDeclaration*>(decl)) {
                    this->GetDecl(decl);
                    llvm::MDNode* node = md.GetMDValue(decl);
                    if (node != NULL) mdn->addOperand(node);
                }
            }
            catch (const detail::ErrorsReceivedException& ex) {
                errors = true;
            }
        }

        if (errors) {
            this->Diag(Ides::Diagnostics::BUILD_FAILED_ERRORS);
            throw std::exception();
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