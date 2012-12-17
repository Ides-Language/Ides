//
//  ASTBinaryExpressionCodeGen.cpp
//  ides
//
//  Created by Sean Edwards on 12/16/12.
//
//

#include <stdio.h>

#include <ides/AST/AST.h>
#include <ides/AST/ASTExpression.h>
#include <ides/AST/ASTConstantExpression.h>

#include <boost/uuid/uuid_generators.hpp> // generators
#include <boost/uuid/uuid_io.hpp>         // streaming operators etc.

#include <llvm/BasicBlock.h>
#include <llvm/Function.h>
#include "llvm/Analysis/Verifier.h"

#include <sstream>
#include <queue>

namespace Ides {
    namespace AST {
        
        
        const Ides::Types::Type* ASTDerefExpression::GetType(llvm::IRBuilder<>* builder, SymbolTable& scope) {
            return this->arg->GetType(builder, scope);
        }
        llvm::Value* ASTDerefExpression::GetValue(llvm::IRBuilder<>* builder, SymbolTable& scope) {
            return NULL;
        }
        
        const Ides::Types::Type* ASTNotExpression::GetType(llvm::IRBuilder<>* builder, SymbolTable& scope) {
            return this->arg->GetType(builder, scope);
        }
        llvm::Value* ASTNotExpression::GetValue(llvm::IRBuilder<>* builder, SymbolTable& scope) {
            return NULL;
        }
        
        const Ides::Types::Type* ASTBnotExpression::GetType(llvm::IRBuilder<>* builder, SymbolTable& scope) {
            return this->arg->GetType(builder, scope);
        }
        llvm::Value* ASTBnotExpression::GetValue(llvm::IRBuilder<>* builder, SymbolTable& scope) {
            return NULL;
        }
        
        const Ides::Types::Type* ASTNegExpression::GetType(llvm::IRBuilder<>* builder, SymbolTable& scope) {
            return this->arg->GetType(builder, scope);
        }
        llvm::Value* ASTNegExpression::GetValue(llvm::IRBuilder<>* builder, SymbolTable& scope) {
            return NULL;
        }
        
        const Ides::Types::Type* ASTPreIncExpression::GetType(llvm::IRBuilder<>* builder, SymbolTable& scope) {
            return this->arg->GetType(builder, scope);
        }
        llvm::Value* ASTPreIncExpression::GetValue(llvm::IRBuilder<>* builder, SymbolTable& scope) {
            return NULL;
        }
        
        const Ides::Types::Type* ASTPreDecExpression::GetType(llvm::IRBuilder<>* builder, SymbolTable& scope) {
            return this->arg->GetType(builder, scope);
        }
        llvm::Value* ASTPreDecExpression::GetValue(llvm::IRBuilder<>* builder, SymbolTable& scope) {
            return NULL;
        }
        
        const Ides::Types::Type* ASTPostIncExpression::GetType(llvm::IRBuilder<>* builder, SymbolTable& scope) {
            return this->arg->GetType(builder, scope);
        }
        llvm::Value* ASTPostIncExpression::GetValue(llvm::IRBuilder<>* builder, SymbolTable& scope) {
            return NULL;
        }
        
        const Ides::Types::Type* ASTPostDecExpression::GetType(llvm::IRBuilder<>* builder, SymbolTable& scope) {
            return this->arg->GetType(builder, scope);
        }
        llvm::Value* ASTPostDecExpression::GetValue(llvm::IRBuilder<>* builder, SymbolTable& scope) {
            return NULL;
        }
        
        const Ides::Types::Type* ASTNewExpression::GetType(llvm::IRBuilder<>* builder, SymbolTable& scope) {
            return this->arg->GetType(builder, scope);
        }
        llvm::Value* ASTNewExpression::GetValue(llvm::IRBuilder<>* builder, SymbolTable& scope) {
            return NULL;
        }
        
        const Ides::Types::Type* ASTThrowExpression::GetType(llvm::IRBuilder<>* builder, SymbolTable& scope) {
            return this->arg->GetType(builder, scope);
        }
        llvm::Value* ASTThrowExpression::GetValue(llvm::IRBuilder<>* builder, SymbolTable& scope) {
            return NULL;
        }
        
        const Ides::Types::Type* ASTReturnExpression::GetType(llvm::IRBuilder<>* builder, SymbolTable& scope) {
            return this->arg->GetType(builder, scope);
        }
        llvm::Value* ASTReturnExpression::GetValue(llvm::IRBuilder<>* builder, SymbolTable& scope) {
            return builder->CreateRet(this->arg->GetValue(builder, scope));
        }
        
    }
}