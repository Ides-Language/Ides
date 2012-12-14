#include <ides/Parsing/Parser.h>

#include <iostream>
#include <iomanip>

#include <parser.hpp>

typedef void* yyscan_t;

namespace Ides {
namespace Parsing {
    
    Ides::AST::AST::Ptr Parse(std::istream& is, const std::string& srcname)
    {
        is.unsetf(std::ios::skipws);

        return Ides::AST::AST::Ptr();
    }
    
    /** class SourceLocation **/
    SourceLocation::SourceLocation(const Iterator& first, const Iterator& last) : first(first), last(last) {}


    /** class Token **/
    Token::Token(int token, const SourceLocation& loc, Line::Ptr line) : loc(loc), line(line)
    {
    }
    
    Token::Ptr Token::Create(int token, const SourceLocation& loc, Line::Ptr line)
    {
        return Token::Ptr(new Token(token, loc, line));
    }
    
    std::string Token::GetText() const
    {
        return std::string(this->loc.first, this->loc.last);
    }

    /** class Line **/
    Line::Line(const SourceLocation& loc) : loc(loc)
    {

    }
    
    std::string Line::GetText() const
    {
        return std::string(this->loc.first, this->loc.last);
    }
    
    /** class Parser **/
    
    Parser::Parser(const std::string& src) : src(src) {
        this->src_iter = this->src.begin();
        this->src_end = this->src.end();
        this->current_line.reset(new Line(SourceLocation(src_iter, src_end)));
    }
    
    int Parser::ReadInput(int max_bytes) {
        char c;
        if (src_iter == src_end) return 0; // EOF
        
        c = *src_iter++;
        return c;
    }
    
    
} // namespace Parsing
} // namespace Ides
