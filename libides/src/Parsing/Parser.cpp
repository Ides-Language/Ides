#include <ides/Parsing/Parser.h>
#include <ides/Parsing/ParserCommon.h>

#include <iostream>
#include <iomanip>
#include <istream>
#include <sstream>
#include <iterator>

#include <parser.hpp>

typedef void* yyscan_t;
int yyparse (Ides::Parsing::Parser* context, Ides::AST::ASTCompilationUnit** program);

namespace Ides {
namespace AST {
    class AST;
    class ASTCompilationUnit;
}
    
namespace Parsing {
    
    using namespace Ides::Diagnostics;
    
    /** class SymbolTable **/
    
    Ides::AST::AST* SymbolTable::LookupRecursive(const Ides::String& symbol) const {
        Ides::AST::AST* ret = this->Lookup(symbol);
        if (ret == NULL && this->parentScope != NULL) {
            ret = parentScope->LookupRecursive(symbol);
        }
        return ret;
    }
    
    Ides::AST::AST* SymbolTable::Lookup(const Ides::String& symbol) const {
        SymbolMap::const_iterator i = this->find(symbol);
        return (i != this->end()) ? i->second : NULL;
    }
    
    /** class Parser **/
    
    Parser::Parser()
    {
    }
    
    
    
    Parser::ParseTree Parser::Parse(std::istream& is, const Ides::String& srcname)
    {
        is.unsetf(std::ios::skipws);
        
        std::stringstream buffer;
        buffer << is.rdbuf();
        src = buffer.str();
        src_name = srcname;
        
        
        publicSymbols.reset(new SymbolTable());
        internalSymbols.reset(new SymbolTable(publicSymbols));
        privateSymbols.reset(new SymbolTable(internalSymbols));
        
        this->src_iter = this->src.begin();
        this->src_end = this->src.end();
        
        this->InitParser();
        
        Ides::AST::ASTCompilationUnit* program = NULL;
        llvm::LLVMContext& llvmctx = llvm::getGlobalContext();
        this->mod = new llvm::Module(srcname, llvmctx);
        this->builder = new llvm::IRBuilder<>(llvmctx);
        
        try {
            yyparse(this, &program);
        } catch (const Ides::Diagnostics::CompileError& ex) {
            this->Issue(ex);
        } catch (const std::exception& ex) {
            while (!localSymbols.empty()) localSymbols.pop();
            throw ex;
        }
        
        return Parser::ParseTree(program);
    }
    
    llvm::Module* Parser::Compile(const ParseTree& t) {
        try {
            t->Compile(*this);
        } catch (const Ides::Diagnostics::CompileError& ex) {
            while (!localSymbols.empty()) localSymbols.pop();
            while (!functionEval.empty()) functionEval.pop();
            throw ex;
        }
        
        assert(this->localSymbols.empty());
        assert(this->functionEval.empty());
        
        return this->GetModule();
    }
    
    void Parser::Issue(const Ides::Diagnostics::CompileIssue& i) {
        std::cerr << i.what() << std::endl;
    }
    
    Parser::~Parser() {
        while (!localSymbols.empty()) localSymbols.pop();
        delete this->builder;
        delete this->mod;
        this->DestroyParser();
    }
    
    int Parser::ReadInput(char *buffer, size_t* numBytesRead, int maxBytesToRead) {
        if (src_iter == src_end) {
            *numBytesRead = 0;
            return 0; // EOF
        }
        
        *buffer = *src_iter++;
        *numBytesRead = 1;
        return 1;
    }
    
    
} // namespace Parsing
} // namespace Ides
