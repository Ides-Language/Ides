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


namespace Ides {
namespace CodeGen {
    
    using namespace Ides::Diagnostics;
    
    CodeGen::CodeGen(clang::IntrusiveRefCntPtr<clang::DiagnosticsEngine> diags, llvm::LLVMContext& lctx, Ides::AST::ASTContext& actx)
        : lctx(lctx), actx(actx), diag(diags)
    {
        this->module = new llvm::Module("Ides Module", lctx);
        this->builder = new llvm::IRBuilder<>(lctx);
    }
    
    CodeGen::~CodeGen() {
        delete builder;
        delete module;
    }
    
    void CodeGen::Compile(Ides::AST::CompilationUnit* ast) { SETTRACE("CodeGen::Compile")
        try {
            ast->Accept(this);
            module->dump();
        } catch (const detail::CodeGenError&) {
            std::cerr << "Build completed with errors." << std::endl;
        }
        functions.clear();
    }
    
    void CodeGen::Visit(Ides::AST::CompilationUnit* ast) {
        Ides::AST::ASTContext::DeclScope typescope(actx, ast);
        
        for (auto i = ast->begin(); i != ast->end(); ++i) {
            i->second->Accept(this);
        }
    }
        
    
}
}