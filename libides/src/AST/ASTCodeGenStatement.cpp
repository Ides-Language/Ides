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
    
    llvm::Value* ASTIfStatement::GetValue(ParseContext& ctx) {
        llvm::BasicBlock* ifblock = llvm::BasicBlock::Create(ctx.GetIRBuilder()->getContext(), "if", (llvm::Function*)ctx.GetEvaluatingFunction()->GetValue(ctx));
        llvm::BasicBlock* elseblock = llvm::BasicBlock::Create(ctx.GetIRBuilder()->getContext(), "else", (llvm::Function*)ctx.GetEvaluatingFunction()->GetValue(ctx));
        llvm::BasicBlock* resumeBlock = llvm::BasicBlock::Create(ctx.GetIRBuilder()->getContext(), "endif", (llvm::Function*)ctx.GetEvaluatingFunction()->GetValue(ctx));
        
        llvm::Value* cond = this->condition->GetType(ctx)->Convert(ctx, this->condition->GetValue(ctx), Ides::Types::Integer1Type::GetSingletonPtr());
        ctx.GetIRBuilder()->CreateCondBr(cond, ifblock, elseblock);
        
        ctx.GetIRBuilder()->SetInsertPoint(ifblock);
        try {
            this->iftrue->GetValue(ctx);
            ctx.GetIRBuilder()->CreateBr(resumeBlock);
        } catch (const Ides::AST::UnitValueException& ex) {
            // Block early-exits function.
        }
        
        ctx.GetIRBuilder()->SetInsertPoint(elseblock);
        if (this->iffalse) {
            try {
                this->iffalse->GetValue(ctx);
                ctx.GetIRBuilder()->CreateBr(resumeBlock);
            } catch (const Ides::AST::UnitValueException& ex) {
                // Block early-exits function.
            }
        } else {
            ctx.GetIRBuilder()->CreateBr(resumeBlock);
        }
        
        ctx.GetIRBuilder()->SetInsertPoint(resumeBlock);
        return NULL;
    }
    
    
    
    
    llvm::Value* ASTWhileStatement::GetValue(ParseContext& ctx) {
        llvm::BasicBlock* whileblock = llvm::BasicBlock::Create(ctx.GetIRBuilder()->getContext(), "while", (llvm::Function*)ctx.GetEvaluatingFunction()->GetValue(ctx));
        llvm::BasicBlock* resumeblock = llvm::BasicBlock::Create(ctx.GetIRBuilder()->getContext(), "endwhile", (llvm::Function*)ctx.GetEvaluatingFunction()->GetValue(ctx));
        
        llvm::Value* cond = this->condition->GetType(ctx)->Convert(ctx, this->condition->GetValue(ctx), Ides::Types::Integer1Type::GetSingletonPtr());
        ctx.GetIRBuilder()->CreateCondBr(cond, whileblock, resumeblock);
        
        ctx.GetIRBuilder()->SetInsertPoint(whileblock);
        try {
            body->GetValue(ctx);
            cond = this->condition->GetType(ctx)->Convert(ctx, this->condition->GetValue(ctx), Ides::Types::Integer1Type::GetSingletonPtr());
            ctx.GetIRBuilder()->CreateCondBr(cond, whileblock, resumeblock);
        } catch (const Ides::AST::UnitValueException& ex) {
            // Block early-exits function.
        }
        
        ctx.GetIRBuilder()->SetInsertPoint(resumeblock);
        return NULL;
    }
    
    
    
    
    
    llvm::Value* ASTForStatement::GetValue(ParseContext& ctx) {
        llvm::BasicBlock* forblock = llvm::BasicBlock::Create(ctx.GetIRBuilder()->getContext(), "for", (llvm::Function*)ctx.GetEvaluatingFunction()->GetValue(ctx));
        llvm::BasicBlock* resumeblock = llvm::BasicBlock::Create(ctx.GetIRBuilder()->getContext(), "endfor", (llvm::Function*)ctx.GetEvaluatingFunction()->GetValue(ctx));
        
        if (this->startexpr) this->startexpr->GetValue(ctx);
        
        llvm::Value* endcond = this->endexpr->GetType(ctx)->Convert(ctx, this->endexpr->GetValue(ctx), Ides::Types::Integer1Type::GetSingletonPtr());
        ctx.GetIRBuilder()->CreateCondBr(endcond, forblock, resumeblock);
        
        ctx.GetIRBuilder()->SetInsertPoint(forblock);
        try {
            body->GetValue(ctx);
            if (this->eachexpr) this->eachexpr->GetValue(ctx);
            endcond = this->endexpr->GetType(ctx)->Convert(ctx, this->endexpr->GetValue(ctx), Ides::Types::Integer1Type::GetSingletonPtr());
            ctx.GetIRBuilder()->CreateCondBr(endcond, forblock, resumeblock);
        } catch (const Ides::AST::UnitValueException& ex) {
            // Block early-exits function.
        }
        
        ctx.GetIRBuilder()->SetInsertPoint(resumeblock);
        return NULL;
    }
    
    
    
    
    
    llvm::Value* ASTCompoundStatement::GetValue(ParseContext &ctx) {
        
        ParseContext::ScopedLocalScope localScope(ctx);
        
        llvm::BasicBlock* scope = llvm::BasicBlock::Create(ctx.GetIRBuilder()->getContext(), "scope", (llvm::Function*)ctx.GetEvaluatingFunction()->GetValue(ctx));
        ctx.GetIRBuilder()->CreateBr(scope);
        ctx.GetIRBuilder()->SetInsertPoint(scope);
        for (auto i = this->begin(); i != this->end(); ++i) {
            try {
                (*i)->GetValue(ctx);
            } catch (const Ides::AST::UnitValueException& ex) {
                if (++i != this->end()) {
                    ctx.Issue(Ides::Diagnostics::CompileWarning("unreachable code", (*i)->exprloc));
                }
                throw ex;
            }
        }
        
        return NULL;
    }
    
}
}