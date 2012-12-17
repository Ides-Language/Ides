#ifndef _IDES_SOURCE_LOCATION_H_
#define _IDES_SOURCE_LOCATION_H_

#include <ides/common.h>
#include <ostream>

namespace Ides {
namespace Diagnostics {
    
    class SourceLocation
    {
    public:
        SourceLocation(const SourceIterator& first,
                       const SourceIterator& last,
                       const SourceIterator& line_start,
                       const SourceIterator& line_end,
                       const Ides::String& source_name,
                       size_t line_num = 1);
        
        SourceLocation() : line_num(1) {}
        
        Ides::String GetText() const;
        
        SourceIterator first;
        SourceIterator last;
        
        SourceIterator line_start;
        SourceIterator line_end;
        
        Ides::String source_name;
        size_t line_num;
    };
    
    class CompileError : public std::exception {
    public:
        CompileError(const Ides::String& msg, const SourceLocation& loc);
        CompileError(const Ides::String& msg, const SourceLocation& loc, const CompileError& from);
        virtual ~CompileError() throw() {}
        
        const char* what() const throw();
        const char* message() const throw() { return msg.c_str(); }
        const SourceLocation& where() const throw() { return loc; }
    private:
        Ides::String from;
        Ides::String msg;
        SourceLocation loc;
    };
}
}

std::ostream& operator<<(std::ostream& os, const Ides::Diagnostics::SourceLocation& loc);
Ides::Diagnostics::SourceLocation operator+(const Ides::Diagnostics::SourceLocation &c1, const Ides::Diagnostics::SourceLocation &c2);

#endif // _IDES_SOURCE_LOCATION_H_
