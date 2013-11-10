//
//  CodeGenDeclaration.cpp
//  ides
//
//  Created by Sean Edwards on 1/2/13.
//
//

#include "CodeGen.h"
#include "llvm/Analysis/Verifier.h"
#include <ides/Diagnostics/Diagnostics.h>

#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Attributes.h>


namespace Ides {
namespace CodeGen {
    
    
    using namespace Ides::Diagnostics;
    
    void CodeGen::Visit(Ides::AST::GlobalVariableDeclaration* ast) { SETTRACE("CodeGen::Visit(GlobalVariableDeclaration)")
        llvm::GlobalVariable* var = NULL;
        const Ides::Types::Type* astType = GetIdesType(ast);
        DIGenerator::DebugScope dbgscope;
        
        auto vi = values.find(ast);
        if (vi != values.end()) {
            var = static_cast<llvm::GlobalVariable*>(vi->second);
        } else {
            var = new llvm::GlobalVariable(*module,
                                           GetLLVMType(astType),
                                           ast->vartype == Ides::AST::VariableDeclaration::DECL_VAL,
                                           llvm::GlobalValue::ExternalLinkage,
                                           0,
                                           ast->GetName());
            
            values.insert(std::make_pair(ast, var));
            
        }
        
        if (this->IsEvaluatingDecl()) {
            
            if (this->dibuilder) {
                auto offset = sman->getFileOffset(ast->exprloc.getBegin());
                
                llvm::DIFile diFile(dibuilder->GetCurrentScope());
                assert(diFile.Verify());
                llvm::MDNode* varNode = dibuilder->createGlobalVariable(ast->GetName(),
                                                                        diFile,
                                                                        sman->getLineNumber(sman->getMainFileID(), offset),
                                                                        dibuilder->GetType(astType),
                                                                        false,
                                                                        var);
                
                llvm::DILexicalBlockFile diScope = dibuilder->createLexicalBlockFile(diFile, diFile);
                dbgscope.SetScope(dibuilder, diScope);
            }
            
            if (ast->initval == NULL) {
                var->setInitializer(llvm::Constant::getNullValue(GetLLVMType(astType)));
                last = var;
                return;
            }
            else if (auto iv = dynamic_cast<Ides::AST::ConstantExpression*>(ast->initval)) {
                // Simple constant initializer. NBD.
                var->setInitializer((llvm::Constant*)GetValue(iv, astType));
                last = var;
                return;
            }
            else {
                // If we're here, we need to evaluate an expression as an initializer.
                // Set up an initializer function.
                var->setInitializer(llvm::Constant::getNullValue(GetLLVMType(astType)));
                
                
                
                llvm::Function* initializer = llvm::Function::Create(llvm::FunctionType::get(llvm::Type::getVoidTy(lctx), false),
                                                                     llvm::GlobalValue::InternalLinkage,
                                                                     "init_" + ast->GetName(),
                                                                     module);
                this->currentStaticInitializer = initializer;
                
                llvm::BasicBlock* oldBB = builder->GetInsertBlock();
                
                llvm::BasicBlock* bb = llvm::BasicBlock::Create(lctx, "entry", initializer);
                builder->SetInsertPoint(bb);
                
                llvm::Value* v = GetValue(ast->initval);
                llvm::Instruction* instr = llvm::cast<llvm::Instruction>(builder->CreateStore(v, var));
                instr->setDebugLoc(GetDebugLoc(ast->initval));
                
                instr = llvm::cast<llvm::Instruction>(builder->CreateRetVoid());
                instr->setDebugLoc(GetDebugLoc(ast->initval));
                
                llvm::verifyFunction(*initializer);

                if (oldBB != NULL)
                    builder->SetInsertPoint(oldBB);
                
                this->globalInitializers.push_back(std::make_pair(this->GetInitializerWeight(WEIGHT_EXPRESSION_VAR), initializer));
            }
        }
        
        last = var;
    }
    
    void CodeGen::Visit(Ides::AST::ArgumentDeclaration* ast) { SETTRACE("CodeGen::Visit(VariableDeclaration)")
        llvm::Value* var;
        auto vi = values.find(ast);
        if (vi != values.end()) {
            var = vi->second;
        }
        else {
            const Ides::Types::Type* varType = GetIdesType(ast);
            llvm::AllocaInst* alloca = builder->CreateAlloca(this->GetLLVMType(varType), 0, ast->GetName());
            alloca->setAlignment(varType->GetAlignment());
            var = alloca;
            
            values.insert(std::make_pair(ast, var));
            
            actx.GetCurrentScope()->AddMember(ast->GetName(), ast);
            
            if (dibuilder != NULL) {
                auto offset = sman->getFileOffset(ast->exprloc.getBegin());
                llvm::DISubprogram currentDIScope(dibuilder->GetCurrentScope());
                assert(currentDIScope.Verify());
                unsigned int lineNum = sman->getLineNumber(this->fid, offset);
                int argNum = ast->argNum + 1;
                llvm::DIVariable diVar = dibuilder->createLocalVariable(llvm::dwarf::DW_TAG_arg_variable,
                                                                        currentDIScope,
                                                                        ast->GetName(),
                                                                        llvm::DIFile(this->diFile),
                                                                        lineNum,
                                                                        dibuilder->GetType(varType),
                                                                        true,
                                                                        0,
                                                                        argNum);
                assert(diVar.Verify());
                auto declInst = dibuilder->insertDeclare(var, diVar, builder->GetInsertBlock());
                this->dbgvalues.insert(std::make_pair(alloca, diVar));
                declInst->setDebugLoc(GetDebugLoc(ast));
            }
        }
        
        last = var;
    }
    
    void CodeGen::Visit(Ides::AST::VariableDeclaration* ast) { SETTRACE("CodeGen::Visit(VariableDeclaration)")
        llvm::Value* var;
        auto vi = values.find(ast);
        if (vi != values.end()) {
            var = vi->second;
        }
        else {
            const Ides::Types::Type* varType = GetIdesType(ast);
            if (ast->vartype == Ides::AST::VariableDeclaration::DECL_VAL) {
                var = GetValue(ast->initval, varType);
            } else {
                llvm::Type* llvmVarType = GetLLVMType(varType->IsFnType() ? varType->PtrType() : varType);
                llvm::AllocaInst* alloca = builder->CreateAlloca(llvmVarType, 0, ast->GetName());
                alloca->setAlignment(varType->GetAlignment());
                var = alloca;

                if (ast->initval != NULL) {
                    llvm::Instruction* instr = llvm::cast<llvm::Instruction>(builder->CreateStore(GetValue(ast->initval, varType), var));
                    if (dibuilder) instr->setDebugLoc(GetDebugLoc(ast));
                }
            }
            
            values.insert(std::make_pair(ast, var));
            actx.GetCurrentScope()->AddMember(ast->GetName(), ast);

            if (dibuilder != NULL) {
                auto offset = sman->getFileOffset(ast->exprloc.getBegin());
                llvm::DIScope currentDIScope(dibuilder->GetCurrentScope());
                assert(currentDIScope.Verify());
                llvm::DIVariable diVar = dibuilder->createLocalVariable(llvm::dwarf::DW_TAG_auto_variable,
                                                                        currentDIScope,
                                                                        ast->GetName(),
                                                                        llvm::DIFile(this->diFile),
                                                                        sman->getLineNumber(this->fid, offset),
                                                                        dibuilder->GetType(varType));
                
                dibuilder->insertDeclare(var, diVar, builder->GetInsertBlock());
                this->dbgvalues.insert(std::make_pair(var, diVar));
            }
        }
        
        last = var;
    }
    
    void CodeGen::Visit(Ides::AST::StructDeclaration* ast) { SETTRACE("CodeGen::Visit(StructDeclaration)")
        Ides::Types::StructType* st = Ides::Types::StructType::GetOrCreate(actx, ast->GetName());
        
        llvm::StructType* llvmst = llvm::cast<llvm::StructType>(this->GetLLVMType(st));
        if (llvmst->isOpaque()) {
            std::vector<llvm::Type*> memberLLVMtypes;
            for (auto i = ast->members.begin(); i != ast->members.end(); ++i) {
                Ides::AST::NamedDeclaration* decl = (Ides::AST::NamedDeclaration*)*i;
                const Ides::Types::Type* memberType = GetIdesType(decl);
                memberLLVMtypes.push_back(this->GetLLVMType(memberType));
            }
            llvmst->setBody(memberLLVMtypes, false);
        }
    }

    void CodeGen::Visit(Ides::AST::OverloadedFunction* ast) { SETTRACE("CodeGen::Visit(OverloadedFunction)")
        for (auto i = ast->begin(); i != ast->end(); ++i) {
            this->GetDecl(*i);
        }
    }

    void CodeGen::Visit(Ides::AST::FunctionDeclaration* ast) { SETTRACE("CodeGen::Visit(FunctionDeclaration)")
        DIGenerator::DebugScope dbgscope;
        llvm::Function* func = NULL;
        const Ides::Types::Type* functionType = GetIdesType(ast);
        auto fi = values.find(ast);
        if (fi != values.end()) {
            func = llvm::cast<llvm::Function>(fi->second);
        }
        else {
            llvm::FunctionType *FT = static_cast<llvm::FunctionType*>(this->GetLLVMType(functionType));
            //func = (llvm::Function*)ctx.GetModule()->getOrInsertFunction(this->GetMangledName(), FT);
            func = llvm::Function::Create(FT, llvm::GlobalValue::ExternalLinkage, ast->GetMangledName(), module);
            //func->setGC("shadow-stack");
            
            if (ast->GetReturnType(actx)->IsEquivalentType(Ides::Types::UnitType::GetSingletonPtr())) {
                func->addFnAttr(llvm::Attribute::NoReturn);
            }
            
            values.insert(std::make_pair(ast, func));
        }
        
        if ((ast->val || ast->body) && this->IsEvaluatingDecl()) {
            LOG(">>>>>>>> Building function " << ast->GetName());
            if (dibuilder) {
                auto offset = sman->getFileOffset(ast->exprloc.getBegin());
                
                llvm::DIFile diCurrentFile(dibuilder->GetCurrentScope());
                assert(diCurrentFile.Verify());
                
                std::vector<llvm::Value*> argDITypes;
                for (auto i = ast->GetArgs().begin(); i != ast->GetArgs().end(); ++i) {
                    const Ides::Types::Type* argType = GetIdesType(*i);
                    llvm::DIType dit(dibuilder->GetType(argType));
                    assert(dit.Verify());
                    argDITypes.push_back(dit);
                }
                llvm::DIArray argTypes = dibuilder->getOrCreateArray(argDITypes);
                
                llvm::DIType funcDIType = dibuilder->createSubroutineType(diCurrentFile, argTypes);
                assert(funcDIType.Verify());
                
                auto funcLineNum = sman->getLineNumber(sman->getMainFileID(), offset);
                llvm::DISubprogram funcNode = dibuilder->createFunction(diCurrentFile,
                                                                        ast->GetName(),
                                                                        ast->GetMangledName(),
                                                                        llvm::DIFile(dibuilder->getCU()),
                                                                        funcLineNum,
                                                                        funcDIType,
                                                                        false,
                                                                        true,
                                                                        funcLineNum,
                                                                        0,
                                                                        false,
                                                                        func);
                
                assert(funcNode.Verify());
                
                dbgscope.SetScope(dibuilder, funcNode);
            }
            
            struct FSM {
                FSM(CodeGen* cg, Ides::AST::FunctionDeclaration* function) : cg(cg) {
                    cg->currentFunctions.push(function);
                }
                
                ~FSM() throw() {
                    cg->currentFunctions.pop();
                }
                CodeGen* cg;
            };
            FSM functionStackManager(this, ast);
            
            Ides::AST::ASTContext::DeclScope typescope(actx, ast);
            
            llvm::BasicBlock* entryblock = llvm::BasicBlock::Create(lctx, "entry", func);
            builder->SetInsertPoint(entryblock);
            
            auto i = ast->GetArgs().begin();
            llvm::Function::arg_iterator ai = func->arg_begin();
            for (; i != ast->GetArgs().end() && ai != func->arg_end(); ++ai, ++i) {
                Ides::AST::ArgumentDeclaration* decl = *i;
                ai->setName(decl->GetName());
                this->GetDecl(decl);
                llvm::Instruction* instr = llvm::cast<llvm::Instruction>(builder->CreateStore(ai, last));
                if (dibuilder) instr->setDebugLoc(GetDebugLoc(decl));
            }
            assert (i == ast->GetArgs().end() && ai == func->arg_end());
            
            
            if (ast->val) {
                if (ast->GetReturnType(actx)->IsEquivalentType(Ides::Types::VoidType::GetSingletonPtr())
                    || ast->GetReturnType(actx)->IsEquivalentType(Ides::Types::UnitType::GetSingletonPtr())) {
                    this->GetValue(ast->val);
                    auto retVal = builder->CreateRetVoid();
                    retVal->setDebugLoc(GetDebugLoc(ast->val));
                } else {
                    const Ides::Types::Type* rettype = ast->GetReturnType(actx);
                    try {
                        auto retVal = builder->CreateRet(GetValue(ast->val, rettype));
                        //retVal->setDebugLoc(GetDebugLoc(ast->val));
                    }
                    catch (const std::exception& ex) {
                        func->removeFromParent();
                        throw ex;
                    }
                }
            }
            else if (ast->body) {
                try {
                    GetValue(ast->body);
                    if (ast->GetReturnType(actx)->IsEquivalentType(Ides::Types::VoidType::GetSingletonPtr())) {
                        // Function didn't return, but it's void, so NBD.
                        llvm::Instruction* instr = (llvm::Instruction*)builder->CreateRetVoid();
                        if (dibuilder) instr->setDebugLoc(GetDebugLoc(ast));
                    } else {
                        func->removeFromParent();
                        throw detail::CodeGenError(*diag, FUNCTION_NO_RETURN, ast->exprloc);
                    }
                } catch (const detail::UnitValueException&) {
                    // Function returned.
                }
            }
            
            LOG("<<<<<<<< Done building function " << ast->GetName());
            llvm::verifyFunction(*func);
        }
        
        assert(func != NULL);
        last = func;
    }

    
}
}
