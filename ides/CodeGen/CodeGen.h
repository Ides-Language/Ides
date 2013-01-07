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

#include <ides/CodeGen/LLVMTypeVisitor.h>

namespace Ides {
    
namespace CodeGen {
    namespace detail {
        class CodeGenError : public Ides::Util::DiagnosticsError {
        public:
            CodeGenError(clang::DiagnosticsEngine& diags, Ides::Diagnostics::DiagIDs diagid, const clang::SourceRange& loc) :
                Ides::Util::DiagnosticsError(diags, diagid, loc) { }
            
            CodeGenError(clang::DiagnosticsEngine& diags, const clang::SourceRange& loc, const Ides::Util::DiagnosticsError& inner) :
                Ides::Util::DiagnosticsError(diags, loc, inner) { }
            
            ~CodeGenError() throw() {}
        };
        
        class UnitValueException { };
        
        class DiagnosticsEmittedException : public std::exception {
        public:
            const char* what() const throw() { return "Code generation completed with errors."; }
            
        };
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
        virtual void Visit(Ides::AST::CompilationUnit* ast);
        
        virtual void Visit(Ides::AST::FunctionDeclaration* ast);
        virtual void Visit(Ides::AST::VariableDeclaration* ast);
        virtual void Visit(Ides::AST::StructDeclaration* ast);
        
        virtual void Visit(Ides::AST::AddressOfExpression* ast);
        virtual void Visit(Ides::AST::DereferenceExpression* ast);
        
        virtual void Visit(Ides::AST::IdentifierExpression* ast);
        virtual void Visit(Ides::AST::DotExpression* ast);
        virtual void Visit(Ides::AST::ReturnExpression* ast);
        virtual void Visit(Ides::AST::FunctionCallExpression* ast);
        
        virtual void Visit(Ides::AST::AssignmentExpression* ast);
        
        virtual void Visit(Ides::AST::Block* ast);
        virtual void Visit(Ides::AST::IfStatement* ast);
        virtual void Visit(Ides::AST::WhileStatement* ast);
        virtual void Visit(Ides::AST::ForStatement* ast);
        
        virtual void Visit(Ides::AST::ConstantStringExpression* ast);
        virtual void Visit(Ides::AST::ConstantCStringExpression* ast);
        virtual void Visit(Ides::AST::ConstantWCStringExpression* ast);
        virtual void Visit(Ides::AST::ConstantLCStringExpression* ast);
        virtual void Visit(Ides::AST::ConstantBoolExpression* ast);
        virtual void Visit(Ides::AST::ConstantIntExpression* ast);
        virtual void Visit(Ides::AST::ConstantFloatExpression* ast);
        
        virtual void Visit(Ides::AST::CastExpression* ast);
        
        virtual void Visit(Ides::AST::BinaryExpression<OP_PLUS>* ast);
        virtual void Visit(Ides::AST::BinaryExpression<OP_MINUS>* ast);
        virtual void Visit(Ides::AST::BinaryExpression<OP_STAR>* ast);
        virtual void Visit(Ides::AST::BinaryExpression<OP_SLASH>* ast);
        virtual void Visit(Ides::AST::BinaryExpression<OP_MOD>* ast);
        
        virtual void Visit(Ides::AST::BinaryExpression<OP_BAND>* ast);
        virtual void Visit(Ides::AST::BinaryExpression<OP_BOR>* ast);
        virtual void Visit(Ides::AST::BinaryExpression<OP_BXOR>* ast);
        
        virtual void Visit(Ides::AST::BinaryExpression<OP_LSHL>* ast);
        virtual void Visit(Ides::AST::BinaryExpression<OP_LSHR>* ast);
        virtual void Visit(Ides::AST::BinaryExpression<OP_ASHL>* ast);
        virtual void Visit(Ides::AST::BinaryExpression<OP_ASHR>* ast);
        
        virtual void Visit(Ides::AST::BinaryExpression<OP_AND>* ast);
        virtual void Visit(Ides::AST::BinaryExpression<OP_OR>* ast);
        
        virtual void Visit(Ides::AST::BinaryExpression<OP_EQ>* ast);
        virtual void Visit(Ides::AST::BinaryExpression<OP_NE>* ast);
        virtual void Visit(Ides::AST::BinaryExpression<OP_LT>* ast);
        virtual void Visit(Ides::AST::BinaryExpression<OP_LE>* ast);
        virtual void Visit(Ides::AST::BinaryExpression<OP_GT>* ast);
        virtual void Visit(Ides::AST::BinaryExpression<OP_GE>* ast);
        
    private:
        
        llvm::Value* GetPtr(Ides::AST::Expression* ast);
        llvm::Value* GetValue(Ides::AST::Expression* ast);
        
        llvm::Value* GetValue(Ides::AST::Expression* ast, const Ides::Types::Type* toType);
        
        llvm::Value* Cast(Ides::AST::Expression* ast, const Ides::Types::Type* toType);
        
        llvm::Type* GetLLVMType(const Ides::Types::Type* ty);
        
        llvm::Function* GetEvaluatingLLVMFunction() { return this->functions[this->currentFunctions.top()]; }
        
        llvm::LLVMContext& lctx;
        Ides::AST::ASTContext& actx;
        
        LLVMTypeVisitor typeVisitor;
        
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
