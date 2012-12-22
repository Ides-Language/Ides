#include <ides/common.h>
#include <ides/Diagnostics/SourceLocation.h>
#include <iostream>

namespace Ides {
namespace Diagnostics {
    /** class SourceLocation **/
    SourceLocation::SourceLocation(const SourceIterator& first,
                                   const SourceIterator& last,
                                   const SourceIterator& line_start,
                                   const SourceIterator& line_end,
                                   const Ides::String& source_name,
                                   size_t line_num) :
    first(first), last(last), source_name(source_name), line_num(line_num) {}
    
    Ides::String SourceLocation::GetText() const {
        return Ides::String(this->first, this->last);
    }
    
    
    CompileError::CompileError(const Ides::String& msg, const SourceLocation& loc) : std::exception(), msg(msg), loc(loc)
    {
    }
    
    CompileError::CompileError(const Ides::String& msg, const SourceLocation& loc, const CompileError& from) : std::exception(), msg(msg), loc(loc)
    {
        std::stringstream buf;
        buf << " from: " << from.loc << std::endl <<
        from.from;
        this->from = buf.str();
    }
    
    const char* CompileError::what() const throw() {
        std::stringstream str;
        str << "Error: " << msg << std::endl <<
        " here: " << this->loc << std::endl <<
        this->from;
        return str.str().c_str();
    }
}
}

std::ostream& operator<<(std::ostream& os, const Ides::Diagnostics::SourceLocation& loc)
{
    os << ">" << Ides::String(loc.first, loc.last) << "< in " << loc.source_name << ":" << loc.line_num;
/*    for (auto i = loc.line_start; i != loc.line_end; ++i) {
        if (i > loc.first && i <= loc.line_end) os << '^';
        else if (*i == '\t') os << '\t';
        else os << ' ';
    }
  */  
    return os;
}

Ides::Diagnostics::SourceLocation operator+(const Ides::Diagnostics::SourceLocation &c1, const Ides::Diagnostics::SourceLocation &c2)
{
    return Ides::Diagnostics::SourceLocation(c1.first, c2.last, c1.line_start, c2.line_end, c1.source_name, c1.line_num);
}
