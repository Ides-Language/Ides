#include <ides/AST/AST.h>
#include <ides/AST/ASTExpression.h>
#include <ides/AST/ASTConstantExpression.h>

namespace Ides {
namespace AST {

    llvm::Value* ASTFunction::CreateMDNode(ParseContext& ctx) {
        std::vector<llvm::Value*> args;
        args.push_back(llvm::MDString::get(ctx.GetContext(), "function"));
        args.push_back(llvm::MDString::get(ctx.GetContext(), this->name->name));
        args.push_back(this->GetReturnType(ctx)->GetMDNode(ctx));
        if (this->args != NULL)
            args.push_back(this->args->GetMDNode(ctx));
        args.push_back(this->func);
        return llvm::MDNode::get(ctx.GetContext(), args);
    }
    
    llvm::Value* ASTList::CreateMDNode(ParseContext &ctx) {
        std::vector<llvm::Value*> args;
        for (auto i = this->begin(); i != this->end(); ++i) {
            args.push_back((*i)->GetMDNode(ctx));
        }
        return llvm::MDNode::get(ctx.GetContext(), args);
    }
    
    llvm::Value* ASTDeclaration::CreateMDNode(ParseContext &ctx) {
        std::vector<llvm::Value*> args;
        args.push_back(llvm::MDString::get(ctx.GetContext(), (this->vartype == ASTDeclaration::DECL_VAL) ? "val" : "var"));
        args.push_back(llvm::MDString::get(ctx.GetContext(), this->name->name));
        args.push_back(this->GetType(ctx)->GetMDNode(ctx));
        
        
        return llvm::MDNode::get(ctx.GetContext(), args);
    }
    
    llvm::Value* ASTStruct::CreateMDNode(ParseContext &ctx) {
        std::vector<llvm::Value*> args;
        args.push_back(llvm::MDString::get(ctx.GetContext(), "struct"));
        args.push_back(llvm::MDString::get(ctx.GetContext(), this->GetName()));
        if (this->members != NULL) {
            args.push_back(this->members->GetMDNode(ctx));
        }
        return llvm::MDNode::get(ctx.GetContext(), args);
    }
    
}
}