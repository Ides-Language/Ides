#include <ides/Parsing/Parser.h>
#include <ides/Parsing/ParserCommon.h>

#include <iostream>
#include <iomanip>
#include <istream>
#include <sstream>
#include <iterator>

#include <parser.hpp>

#include <ides/AST/AST.h>

typedef void* yyscan_t;
int yyparse (Ides::Parsing::Parser* context, Ides::AST::ASTCompilationUnit** program);

namespace Ides {
namespace Parsing {
    
    using namespace Ides::Diagnostics;
    
    Ides::AST::AST* Parse(std::istream& is, const Ides::String& srcname)
    {
        is.unsetf(std::ios::skipws);
        
        std::stringstream buffer;
        buffer << is.rdbuf();
        Parser p(buffer.str(), srcname);

        return NULL;
    }
    
    /** class Parser **/
    
    Parser::Parser(const Ides::String& src, const Ides::String& srcname) :
        src_name(srcname), src(src)
    {
        this->src_iter = this->src.begin();
        this->src_end = this->src.end();
        
        this->InitParser();
        
        Ides::AST::ASTCompilationUnit* program = NULL;
        try {
            yyparse(this, &program);
            
            if (program) {
                std::cout << "Original source: " << std::endl;
                std::cout << src << std::endl << std::endl;
                
                std::cout << "C Header: " << std::endl;
                std::cout << program->GetCHeader() << std::endl << std::endl;
                
                std::cout << "LLVM Module: " << std::endl;
                llvm::Module m(srcname, llvm::getGlobalContext());
                
                program->Compile(m);
                
                //std::cout << program->GetDOT() << std::endl;
                
                delete program;
            }
        } catch (const std::exception& ex) {
            std::cerr << ex.what() << std::endl;
        }
    }
    
    Parser::~Parser() {
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
