//
//  CodeGen.h
//  ides
//
//  Created by Sean Edwards on 12/30/12.
//
//

#ifndef __ides__CodeGen__
#define __ides__CodeGen__

#include <stack>
#include <ides/common.h>

#include <ides/AST/AST.h>
#include <ides/AST/Declaration.h>
#include <ides/AST/Statement.h>
#include <ides/AST/Expression.h>

#include <ides/ASTVisitor/ASTVisitor.h>

#include <ides/Parsing/ParseContext.h>

#include <ides/Diagnostics/Diagnostics.h>

namespace Ides {
    
namespace CodeGen {
    namespace detail {
        class CodeGenError { };
        class UnitValueException { };
    }
    
    class CodeGen : public Ides::AST::Visitor {
        friend struct FSM;
    public:
        CodeGen(clang::IntrusiveRefCntPtr<clang::DiagnosticsEngine> diags, llvm::LLVMContext& lctx, Ides::AST::ASTContext& actx);
        ~CodeGen();
        
        void Compile(Ides::AST::CompilationUnit* ast);
        llvm::Module* GetModule() const { return module; }
        
        clang::DiagnosticBuilder Diag(Ides::Diagnostics::DiagIDs id) {
            return Ides::Diagnostics::Diag(*diag, id);
        }
        
        clang::DiagnosticBuilder Diag(Ides::Diagnostics::DiagIDs id, clang::SourceLocation loc) {
            return Ides::Diagnostics::Diag(*diag, id, loc);
        }
        
        clang::DiagnosticBuilder Diag(Ides::Diagnostics::DiagIDs id, Ides::AST::AST* ast) {
            return Ides::Diagnostics::Diag(*diag, id, ast->exprloc.getBegin());
        }
        
    public:
        void Visit(Ides::AST::CompilationUnit* ast);
        
        void Visit(Ides::AST::FunctionDeclaration* ast);
        void Visit(Ides::AST::StructDeclaration* ast);
        void Visit(Ides::AST::VariableDeclaration* ast);
        
        void Visit(Ides::AST::IdentifierExpression* ast);
        void Visit(Ides::AST::DotExpression* ast);
        void Visit(Ides::AST::ReturnExpression* ast);
        
        void Visit(Ides::AST::Block* ast);
        void Visit(Ides::AST::IfStatement* ast);
        void Visit(Ides::AST::WhileStatement* ast);
        void Visit(Ides::AST::ForStatement* ast);
        
        void Visit(Ides::AST::ConstantStringExpression* ast);
        void Visit(Ides::AST::ConstantCStringExpression* ast);
        void Visit(Ides::AST::ConstantWCStringExpression* ast);
        void Visit(Ides::AST::ConstantLCStringExpression* ast);
        void Visit(Ides::AST::ConstantBoolExpression* ast);
        void Visit(Ides::AST::ConstantCharExpression* ast);
        void Visit(Ides::AST::ConstantIntExpression* ast);
        void Visit(Ides::AST::ConstantFloatExpression* ast);
        
    private:
        
        llvm::Function* GetEvaluatingLLVMFunction() { return this->functions[this->currentFunctions.top()]; }
        
        llvm::LLVMContext& lctx;
        Ides::AST::ASTContext& actx;
        
        llvm::IRBuilder<>* builder;
        llvm::Module* module;
        
        boost::unordered_map<Ides::AST::FunctionDeclaration*, llvm::Function*> functions;
        boost::unordered_map<Ides::AST::VariableDeclaration*, llvm::Value*> variables;
        
        std::stack<Ides::AST::FunctionDeclaration*> currentFunctions;
        
        llvm::Value* last;
        
        clang::IntrusiveRefCntPtr<clang::DiagnosticsEngine> diag;
    };
    
}
}

#endif /* defined(__ides__CodeGen__) */
