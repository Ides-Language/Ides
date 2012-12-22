#ifndef _IDES_PARSER_H_
#define _IDES_PARSER_H_

#include <string>
#include <boost/unordered_map.hpp>
#include <ostream>
#include <stack>
#include <list>
#include <ides/common.h>
#include <boost/shared_ptr.hpp>
#include <memory>

#include <ides/Diagnostics/SourceLocation.h>

namespace Ides {
    namespace AST {
        class AST;
        class ASTFunction;
        class ASTCompilationUnit;
    }
    namespace Types {
        class Type;
    }
    
    namespace Parsing {
        
        typedef boost::unordered_map<Ides::String, Ides::AST::AST* > SymbolMap;
        
        class IDES_EXPORTS SymbolTable : public SymbolMap {
        public:
            typedef boost::shared_ptr<SymbolTable> Ptr;
            SymbolTable() { }
            SymbolTable(SymbolTable::Ptr parent) : parentScope(parent) { }
            
            Ides::AST::AST* LookupRecursive(const Ides::String& symbol) const;
            Ides::AST::AST* Lookup(const Ides::String& symbol) const;
            
            SymbolTable::Ptr GetParentScope() { return parentScope; }
            void SetParentScope(SymbolTable::Ptr scope) { parentScope = scope; }
        private:
            SymbolTable::Ptr parentScope;
        }; // class SymbolTable
        
        
        class Parser {
        public:
            typedef std::auto_ptr<Ides::AST::ASTCompilationUnit> ParseTree;
            
            Parser();
            ~Parser();
            
            ParseTree Parse(std::istream& is, const Ides::String& srcname);
            llvm::Module* Compile(const ParseTree& t);
            
            void* GetScanner() const { return this->scanner; }
            
            int ReadInput(char *buffer, size_t* numBytesRead, int maxBytesToRead);
            
            const Ides::String& GetSource() const { return this->src; }
            const Ides::String& GetSourceName() const { return this->src_name; }
            
            std::ostream& StreamCHeader();
            std::ostream& StreamASTGraph();
            std::ostream& StreamBytecode();
            std::ostream& StreamDebug() { return std::cout; }
            
            llvm::IRBuilder<>* GetIRBuilder() { return builder; }
            llvm::Module* GetModule() { return mod; }
            llvm::LLVMContext& GetContext() { return GetIRBuilder()->getContext(); }
            
            SymbolTable::Ptr GetPublicSymbols() { return publicSymbols; }
            SymbolTable::Ptr GetInternalSymbols() { return internalSymbols; }
            SymbolTable::Ptr GetPrivateSymbols() { return privateSymbols; }
            SymbolTable::Ptr GetLocalSymbols() { return localSymbols.top(); }
            
            void PushLocalScope() {
                SymbolTable::Ptr oldlocals = localSymbols.empty() ? privateSymbols : localSymbols.top();
                localSymbols.push(SymbolTable::Ptr(new SymbolTable()));
                GetLocalSymbols()->SetParentScope(oldlocals);
            }
            void PopLocalScope() { localSymbols.pop(); }
            
            void PushFunction(Ides::AST::ASTFunction* func) { functionEval.push(func); }
            void PopFunction() { functionEval.pop(); }
            Ides::AST::ASTFunction* GetEvaluatingFunction() const { return functionEval.top(); }
            
            const Ides::SourceIterator& GetSourceIterator() const { return this->src_iter; }
            
            void Issue(const Ides::Diagnostics::CompileIssue& i);
            
            class ScopedLocalScope {
            public:
                ScopedLocalScope(Parser& ctx) : ctx(ctx) { ctx.PushLocalScope(); }
                ~ScopedLocalScope() { ctx.PopLocalScope(); }
            private:
                Parser& ctx;
            };
            
            class ScopedFunction {
            public:
                ScopedFunction(Parser& ctx, Ides::AST::ASTFunction* func) : ctx(ctx) { ctx.PushFunction(func); }
                ~ScopedFunction() { ctx.PopFunction(); }
            private:
                Parser& ctx;
            };
            
        protected:
            void InitParser();
            void DestroyParser();
        private:
            void* scanner;
            Ides::String src_name;
            Ides::String src;
            Ides::SourceIterator src_iter;
            Ides::SourceIterator src_end;
            
            SymbolTable::Ptr publicSymbols;
            SymbolTable::Ptr internalSymbols;
            SymbolTable::Ptr privateSymbols;
            std::stack<SymbolTable::Ptr> localSymbols;
            std::stack<Ides::AST::ASTFunction*> functionEval;
            
            
            llvm::Module* mod;
            llvm::IRBuilder<>* builder;
        };
        
        typedef Parser ParseContext;
    }
}


#endif // _IDES_PARSER_H_
