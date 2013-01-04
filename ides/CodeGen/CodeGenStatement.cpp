//
//  CodeGenStatement.cpp
//  ides
//
//  Created by Sean Edwards on 12/31/12.
//
//

#include "CodeGen.h"
#include <ides/Diagnostics/Diagnostics.h>

#include <ides/AST/Statement.h>
#include <llvm/BasicBlock.h>


namespace Ides {
namespace CodeGen {
    
    using namespace Ides::Diagnostics;
    
    void CodeGen::Visit(Ides::AST::IfStatement* ast) { SETTRACE("CodeGen::Visit(IfStatement)")
        Ides::AST::ASTContext::DeclScope typescope(actx, ast);
        
        llvm::BasicBlock* ifblock = llvm::BasicBlock::Create(lctx, "if", GetEvaluatingLLVMFunction());
        llvm::BasicBlock* elseblock = llvm::BasicBlock::Create(lctx, "else", GetEvaluatingLLVMFunction());
        llvm::BasicBlock* resumeBlock = llvm::BasicBlock::Create(lctx, "endif", GetEvaluatingLLVMFunction());
        
        bool blockReturn[2];
        
        ast->condition->Accept(this);
        builder->CreateCondBr(last, ifblock, elseblock);
        
        builder->SetInsertPoint(ifblock);
        try {
            ast->iftrue->Accept(this);
            builder->CreateBr(resumeBlock);
            blockReturn[0] = false;
        } catch (const detail::UnitValueException&) {
            blockReturn[0] = true;
            // Block early-exits function.
        }
        
        builder->SetInsertPoint(elseblock);
        if (ast->iffalse) {
            try {
                ast->iffalse->Accept(this);
                builder->CreateBr(resumeBlock);
                blockReturn[1] = false;
            } catch (const detail::UnitValueException&) {
                blockReturn[1] = true;
                // Block early-exits function.
            }
        } else {
            builder->CreateBr(resumeBlock);
        }
        
        if (blockReturn[0] && blockReturn[1]) {
            // Both conditions return from the function.
            // Scrap the resume block and indicate that we're done.
            resumeBlock->removeFromParent();
            throw detail::UnitValueException();
        } else {
            builder->SetInsertPoint(resumeBlock);
        }
    }
    
    void CodeGen::Visit(Ides::AST::WhileStatement* ast) { SETTRACE("CodeGen::Visit(WhileStatement)")
        Ides::AST::ASTContext::DeclScope typescope(actx, ast);
        
        llvm::BasicBlock* whileblock = llvm::BasicBlock::Create(lctx, "while", GetEvaluatingLLVMFunction());
        llvm::BasicBlock* resumeblock = llvm::BasicBlock::Create(lctx, "endwhile", GetEvaluatingLLVMFunction());
        
        builder->CreateCondBr(GetValue(ast->condition), whileblock, resumeblock);
        
        builder->SetInsertPoint(whileblock);
        try {
            ast->body->Accept(this);
            builder->CreateCondBr(GetValue(ast->condition), whileblock, resumeblock);
        } catch (const detail::UnitValueException& ex) {
            // Block early-exits function.
            // Don't propogate the Unit value because we may not execute at all.
        }
        builder->SetInsertPoint(resumeblock);
    }
    
    void CodeGen::Visit(Ides::AST::ForStatement* ast) { SETTRACE("CodeGen::Visit(ForStatement)")
        Ides::AST::ASTContext::DeclScope typescope(actx, ast);
        
        llvm::BasicBlock* forblock = llvm::BasicBlock::Create(lctx, "for", GetEvaluatingLLVMFunction());
        llvm::BasicBlock* resumeblock = llvm::BasicBlock::Create(lctx, "endfor", GetEvaluatingLLVMFunction());
        
        if (ast->startexpr) ast->startexpr->Accept(this);
        
        builder->CreateCondBr(GetValue(ast->endexpr), forblock, resumeblock);
        
        builder->SetInsertPoint(forblock);
        try {
            ast->body->Accept(this);
            if (ast->eachexpr) GetValue(ast->eachexpr);
            builder->CreateCondBr(GetValue(ast->endexpr), forblock, resumeblock);
        } catch (const detail::UnitValueException& ex) {
            // Block early-exits function.
            // Don't propogate the Unit value because we may not execute at all.
        }
        
        builder->SetInsertPoint(resumeblock);
    }
    
    void CodeGen::Visit(Ides::AST::Block* ast) { SETTRACE("CodeGen::Visit(Block)")
        Ides::AST::ASTContext::DeclScope typescope(actx, ast);
        
        //ParseContext::ScopedLocalScope localScope(ctx);
        
        llvm::BasicBlock* scope = llvm::BasicBlock::Create(lctx, "scope", GetEvaluatingLLVMFunction());
        builder->CreateBr(scope);
        builder->SetInsertPoint(scope);
        for (auto i = ast->statements.begin(); i != ast->statements.end(); ++i) {
            try {
                (*i)->Accept(this);
            } catch (const detail::UnitValueException& ex) {
                if (++i != ast->statements.end()) {
                    Diag(BLOCK_UNREACHABLE_CODE, *i);
                }
                throw ex;
            }
        }
    }
    
}
}