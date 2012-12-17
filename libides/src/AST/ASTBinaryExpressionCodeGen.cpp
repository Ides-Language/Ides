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
    
    const Ides::Types::Type* ASTEqExpression::GetType(llvm::IRBuilder<>* builder, SymbolTable& scope) {
        return NULL;
    }
    llvm::Value* ASTEqExpression::GetValue(llvm::IRBuilder<>* builder, SymbolTable& scope) {
        return builder->CreateAdd(this->lhs->GetValue(builder, scope), this->rhs->GetValue(builder, scope));
    }
    
    const Ides::Types::Type* ASTAddExpression::GetType(llvm::IRBuilder<>* builder, SymbolTable& scope) {
        return lhs->GetType(builder, scope);
    }
    llvm::Value* ASTAddExpression::GetValue(llvm::IRBuilder<>* builder, SymbolTable& scope) {
        return builder->CreateAdd(lhs->GetValue(builder, scope), rhs->GetValue(builder, scope));
    }
    
    const Ides::Types::Type* ASTSubExpression::GetType(llvm::IRBuilder<>* builder, SymbolTable& scope) {
        return NULL;
    }
    llvm::Value* ASTSubExpression::GetValue(llvm::IRBuilder<>* builder, SymbolTable& scope) {
        return NULL;
    }
    
    const Ides::Types::Type* ASTMulExpression::GetType(llvm::IRBuilder<>* builder, SymbolTable& scope) {
        return NULL;
    }
    llvm::Value* ASTMulExpression::GetValue(llvm::IRBuilder<>* builder, SymbolTable& scope) {
        return NULL;
    }
    
    const Ides::Types::Type* ASTDivExpression::GetType(llvm::IRBuilder<>* builder, SymbolTable& scope) {
        return NULL;
    }
    llvm::Value* ASTDivExpression::GetValue(llvm::IRBuilder<>* builder, SymbolTable& scope) {
        return NULL;
    }
    
    const Ides::Types::Type* ASTModExpression::GetType(llvm::IRBuilder<>* builder, SymbolTable& scope) {
        return NULL;
    }
    llvm::Value* ASTModExpression::GetValue(llvm::IRBuilder<>* builder, SymbolTable& scope) {
        return NULL;
    }
    
    const Ides::Types::Type* ASTBandExpression::GetType(llvm::IRBuilder<>* builder, SymbolTable& scope) {
        return NULL;
    }
    llvm::Value* ASTBandExpression::GetValue(llvm::IRBuilder<>* builder, SymbolTable& scope) {
        return builder->CreateAnd(this->lhs->GetValue(builder, scope), this->rhs->GetValue(builder, scope));
    }
    
    const Ides::Types::Type* ASTBorExpression::GetType(llvm::IRBuilder<>* builder, SymbolTable& scope) {
        return NULL;
    }
    llvm::Value* ASTBorExpression::GetValue(llvm::IRBuilder<>* builder, SymbolTable& scope) {
        return builder->CreateOr(this->lhs->GetValue(builder, scope), this->rhs->GetValue(builder, scope));
    }
    
    const Ides::Types::Type* ASTBxorExpression::GetType(llvm::IRBuilder<>* builder, SymbolTable& scope) {
        return NULL;
    }
    llvm::Value* ASTBxorExpression::GetValue(llvm::IRBuilder<>* builder, SymbolTable& scope) {
        return builder->CreateXor(this->lhs->GetValue(builder, scope), this->rhs->GetValue(builder, scope));
    }
    
    const Ides::Types::Type* ASTCoalesceExpression::GetType(llvm::IRBuilder<>* builder, SymbolTable& scope) {
        return NULL;
    }
    llvm::Value* ASTCoalesceExpression::GetValue(llvm::IRBuilder<>* builder, SymbolTable& scope) {
        return NULL;
    }
    
    const Ides::Types::Type* ASTCastExpression::GetType(llvm::IRBuilder<>* builder, SymbolTable& scope) {
        return NULL;
    }
    llvm::Value* ASTCastExpression::GetValue(llvm::IRBuilder<>* builder, SymbolTable& scope) {
        return NULL;
    }
    
    const Ides::Types::Type* ASTDictPairExpression::GetType(llvm::IRBuilder<>* builder, SymbolTable& scope) {
        return NULL;
    }
    llvm::Value* ASTDictPairExpression::GetValue(llvm::IRBuilder<>* builder, SymbolTable& scope) {
        return NULL;
    }
    
    
}
}