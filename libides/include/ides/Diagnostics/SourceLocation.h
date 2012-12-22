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
    
    class CompileIssue : public std::exception {
    public:
        CompileIssue(const Ides::String& msg, const SourceLocation& loc);
        CompileIssue(const Ides::String& msg, const SourceLocation& loc, const CompileIssue& from);
        virtual ~CompileIssue() throw() {}
        
        enum IssueSeverity {
            ERROR,
            WARNING,
            NOTICE
        };
        
        const char* what() const throw();
        const char* message() const throw() { return msg.c_str(); }
        const SourceLocation& where() const throw() { return loc; }
        virtual const IssueSeverity severity() const { return NOTICE; }
    private:
        Ides::String from;
        Ides::String msg;
        SourceLocation loc;
    };
    
    class CompileError : public CompileIssue {
    public:
        CompileError(const Ides::String& msg, const SourceLocation& loc) : CompileIssue(msg, loc) { }
        CompileError(const Ides::String& msg, const SourceLocation& loc, const CompileError& from) : CompileIssue(msg, loc, from) { }
        virtual ~CompileError() throw() {}
        virtual const IssueSeverity severity() const { return CompileIssue::ERROR; }
    };
    
    class CompileWarning : public CompileIssue {
    public:
        CompileWarning(const Ides::String& msg, const SourceLocation& loc) : CompileIssue(msg, loc) { }
        CompileWarning(const Ides::String& msg, const SourceLocation& loc, const CompileError& from) : CompileIssue(msg, loc, from) { }
        virtual ~CompileWarning() throw() {}
        virtual const IssueSeverity severity() const { return CompileIssue::WARNING; }
    };
}
}

std::ostream& operator<<(std::ostream& os, const Ides::Diagnostics::SourceLocation& loc);
Ides::Diagnostics::SourceLocation operator+(const Ides::Diagnostics::SourceLocation &c1, const Ides::Diagnostics::SourceLocation &c2);

#endif // _IDES_SOURCE_LOCATION_H_
