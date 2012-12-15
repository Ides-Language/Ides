#include <ides/Parsing/Parser.h>

#include <iostream>
#include <iomanip>
#include <istream>
#include <sstream>
#include <iterator>

#include <parser.hpp>

typedef void* yyscan_t;
int yyparse (Ides::Parsing::Parser* context);

namespace Ides {
namespace Parsing {
    
    Ides::AST::AST* Parse(std::istream& is, const Ides::String& srcname)
    {
        is.unsetf(std::ios::skipws);
        
        std::stringstream buffer;
        buffer << is.rdbuf();
        Parser p(buffer.str());

        return NULL;
    }
    
    /** class SourceLocation **/
    SourceLocation::SourceLocation(const Iterator& first, const Iterator& last) : first(first), last(last) {}

    /** class Line **/
    Line::Line(const SourceLocation& loc) : loc(loc)
    {

    }
    
    Ides::String Line::GetText() const
    {
        return Ides::String(this->loc.first, this->loc.last);
    }
    
    /** class Parser **/
    
    Parser::Parser(const Ides::String& src) : src(src) {
        this->src_iter = this->src.begin();
        this->src_end = this->src.end();
        this->current_line.reset(new Line(SourceLocation(src_iter, src_end)));
        
        this->InitParser();
        
        yyparse(this);
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


std::ostream& operator<<(std::ostream& os, const Ides::Parsing::SourceLocation& loc)
{
    os << Ides::String(loc.first, loc.last);
    return os;
}
