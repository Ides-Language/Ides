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
    
    
    void CodeGen::Visit(Ides::AST::AssignmentExpression* ast) { SETTRACE("CodeGen::Visit(AssignmentExpression)")
        llvm::Value* toExpr = GetPtr(ast->GetLHS());
        builder->CreateStore(GetValue(ast->GetRHS(), ast->GetLHS()->GetType(actx)), toExpr);
        last = toExpr;
    }
    
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
        if (exprtype == NULL) {
            throw detail::CodeGenError();
        }
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
            llvm::Value* retVal = GetValue(ast->GetRetVal());
            builder->CreateRet(retVal);
        }
        else {
            // Returning from function with return type.
            builder->CreateRet(GetValue(ast->GetRetVal(), ast->GetRetType(actx)));
        }
        throw detail::UnitValueException();
    }
    
    
    void CodeGen::Visit(Ides::AST::DotExpression* ast) { SETTRACE("CodeGen::Visit(DotExpression)")
        llvm::Value* ptr = GetPtr(&ast->GetExpression());
        
        const Ides::Types::Type* exprtype = ast->GetExpression().GetType(actx);
        Ides::AST::Declaration* decl = exprtype->GetMember(actx, *ast->GetToken());
        
        if (decl == NULL) {
            Diag(UNKNOWN_MEMBER, ast) << *ast->GetToken() << exprtype->ToString();
            throw detail::CodeGenError();
        }
        
        if (const Ides::Types::StructType* st = dynamic_cast<const Ides::Types::StructType*>(ast->GetExpression().GetType(actx))) {
            int memberidx = st->GetMemberIndex(*ast->GetToken());
            if (memberidx == -1) {
                Diag(UNKNOWN_MEMBER, ast) << *ast->GetToken() << exprtype->ToString();
                throw detail::CodeGenError();
            }
            last = builder->CreateStructGEP(ptr, memberidx, exprtype->ToString() + "." + *ast->GetToken());
            // No such member
            return;
        }
        else {
        }
    }
    
    
    void CodeGen::Visit(Ides::AST::FunctionCallExpression* ast) { SETTRACE("CodeGen::Visit(FunctionCallExpression)")
        const Ides::Types::FunctionType* function = static_cast<const Ides::Types::FunctionType*>(ast->GetFunction()->GetType(actx));
        if (function == NULL) throw detail::CodeGenError();
        
        const Ides::AST::ExpressionList& args = ast->GetArgs();
        llvm::Function* func = static_cast<llvm::Function*>(GetPtr(ast->GetFunction()));
        std::vector<llvm::Value*> fnargs;
        auto i = args.begin();
        auto defi = function->argTypes.begin();
        for (; i != args.end() && defi != function->argTypes.end(); ++i, ++defi) {
            fnargs.push_back(GetValue(*i, *defi));
        }
        
        if (function->argTypes.size() > args.size()) {
            Diag(CALL_INSUFFICIENT_ARGS, ast) << (int)function->argTypes.size() << (int)args.size();
            throw detail::CodeGenError();
        } else if (function->argTypes.size() < args.size()) {
            Diag(CALL_TOO_MANY_ARGS, ast) << (int)function->argTypes.size() << (int)args.size();
            throw detail::CodeGenError();
        }
        last = builder->CreateCall(func, fnargs);
    }
    
    
    void CodeGen::Visit(Ides::AST::AddressOfExpression* ast) { SETTRACE("CodeGen::Visit(AddressOfExpression)")
        llvm::Value* ptr = builder->CreateAlloca(GetLLVMType(ast->arg->GetType(actx)), 0, "addrof");
        builder->CreateStore(GetValue(ast->arg.get()), ptr);
        last = ptr;
    }
    
    void CodeGen::Visit(Ides::AST::DereferenceExpression* ast) { SETTRACE("CodeGen::Visit(DereferenceExpression)")
        const Ides::Types::Type* astType = ast->arg->GetType(actx);
        if (dynamic_cast<const Ides::Types::PointerType*>(astType)) {
            last = builder->CreateLoad(this->GetPtr(ast->arg.get()), "deref");
            return;
        }
        Diag(OP_NO_SUCH_OPERATOR, ast) << "*" << astType->ToString();
        throw detail::CodeGenError();
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
        last = llvm::ConstantInt::get(this->GetLLVMType(ast->GetType(actx)), ast->GetValue());
    }
    
    void CodeGen::Visit(Ides::AST::ConstantIntExpression* ast) {
        last = llvm::ConstantInt::get(this->GetLLVMType(ast->GetType(actx)), ast->GetValue());
    }
    
    void CodeGen::Visit(Ides::AST::ConstantFloatExpression* ast) {
        last = llvm::ConstantFP::get(this->GetLLVMType(ast->GetType(actx)), ast->GetValue());
    }


}
}