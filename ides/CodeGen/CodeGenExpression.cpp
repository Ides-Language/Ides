//
//  CodeGenExpression.cpp
//  ides
//
//  Created by Sean Edwards on 1/2/13.
//
//

#include "CodeGen.h"
#include <ides/Diagnostics/Diagnostics.h>

#include <ides/AST/ConstantExpression.h>
#include <ides/AST/Expression.h>
#include <llvm/BasicBlock.h>


namespace Ides {
namespace CodeGen {
    
    
    using namespace Ides::Diagnostics;
    
    void CodeGen::Visit(Ides::AST::IdentifierExpression* ast) { SETTRACE("CodeGen::Visit(IdentifierExpression)")
        Ides::AST::Declaration* decl = actx.GetCurrentScope()->GetMember(actx, ast->GetName());
        if (decl) {
            decl->Accept(this);
            return;
        }
        
        Diag(UNKNOWN_IDENTIFIER) << ast->GetName();
        throw detail::CodeGenError();
    }
    
    void CodeGen::Visit(Ides::AST::ReturnExpression* ast) { SETTRACE("CodeGen::Visit(ReturnExpression)")
        const Ides::Types::Type* exprtype = ast->GetRetType(actx);
        if (exprtype == NULL) throw detail::CodeGenError();
        const Ides::Types::Type* funcrettype = this->currentFunctions.top()->GetReturnType(actx);
        if (funcrettype == NULL) {
            throw detail::CodeGenError();
        }
        else if (funcrettype == Ides::Types::VoidType::GetSingletonPtr()) {
            if (ast->GetRetVal() != NULL) {
                Diag(RETURN_FROM_VOID, ast->exprloc.getBegin());
                throw detail::CodeGenError();
            }
            last = builder->CreateRetVoid();
        }
        else if (funcrettype == Ides::Types::UnitType::GetSingletonPtr()) {
            Diag(RETURN_FROM_UNIT, ast->exprloc.getBegin());
            throw detail::CodeGenError();
        }
        else if (ast->GetRetVal() == NULL && funcrettype != Ides::Types::VoidType::GetSingletonPtr()) {
            Diag(RETURN_NO_EXPRESSION, ast->exprloc.getBegin());
            throw detail::CodeGenError();
        }
        else if (exprtype->IsEquivalentType(funcrettype)) {
            ast->GetRetVal()->Accept(this);
            builder->CreateRet(last);
        }
        else {
            try {
                //ctx.GetIRBuilder()->CreateRet(this->retval->GetValue(ctx, funcrettype));
            } catch (const std::exception& ex) {
                //throw Ides::Diagnostics::CompileError(ex.what(), this->exprloc);
            }
        }
        throw detail::UnitValueException();
    }
    
    
    void CodeGen::Visit(Ides::AST::DotExpression* ast) {
        ast->GetExpression().Accept(this);
        //llvm::Value* lhs = last;
        ast->GetExpression().GetMember(actx, *ast->GetToken())->Accept(this);
    }
    
    
    
    void CodeGen::Visit(Ides::AST::ConstantStringExpression* ast) {
        
    }
    
    void CodeGen::Visit(Ides::AST::ConstantCStringExpression* ast) {
        last = builder->CreateGlobalStringPtr(ast->GetBuffer().str());
    }
    
    void CodeGen::Visit(Ides::AST::ConstantWCStringExpression* ast) {
        
    }
    
    void CodeGen::Visit(Ides::AST::ConstantLCStringExpression* ast) {
        
    }
    
    void CodeGen::Visit(Ides::AST::ConstantBoolExpression* ast) {
        last = ast->GetValue() ? llvm::ConstantInt::getTrue(lctx) : llvm::ConstantInt::getFalse(lctx);
    }
    
    void CodeGen::Visit(Ides::AST::ConstantCharExpression* ast) {
        last = llvm::ConstantInt::get(ast->GetType(actx)->GetLLVMType(actx), ast->GetValue());
    }
    
    void CodeGen::Visit(Ides::AST::ConstantIntExpression* ast) {
        last = llvm::ConstantInt::get(ast->GetType(actx)->GetLLVMType(actx), ast->GetValue());
    }
    
    void CodeGen::Visit(Ides::AST::ConstantFloatExpression* ast) {
        last = llvm::ConstantFP::get(ast->GetType(actx)->GetLLVMType(actx), ast->GetValue());
    }


}
}