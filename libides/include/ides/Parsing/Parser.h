#ifndef _IDES_PARSER_H_
#define _IDES_PARSER_H_

#include <string>
#include <map>
#include <ostream>
#include <stack>
#include <ides/common.h>
#include <boost/shared_ptr.hpp>

#include <ides/Diagnostics/SourceLocation.h>

namespace Ides {
    namespace AST {
        class AST;
    }
    
    namespace Parsing {
        
        Ides::AST::AST* Parse(std::istream& is, const Ides::String& srcname);
        
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
            
            Parser(const Ides::String& src, const Ides::String& srcname);
            ~Parser();
            
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
            
            const Ides::SourceIterator& GetSourceIterator() const { return this->src_iter; }
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
            
            
            llvm::Module* mod;
            llvm::IRBuilder<>* builder;
        };
    }
}


#endif // _IDES_PARSER_H_
