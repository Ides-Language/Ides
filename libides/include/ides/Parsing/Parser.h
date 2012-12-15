#ifndef _IDES_PARSER_H_
#define _IDES_PARSER_H_

#include <string>
#include <ides/common.h>
#include <boost/shared_ptr.hpp>

namespace Ides {
    namespace AST {
        class AST;
    }
    
    namespace Parsing {
        typedef Ides::String::const_iterator Iterator;
        
        Ides::AST::AST* Parse(std::istream& is, const Ides::String& srcname);
        
        class SourceLocation
        {
        public:
            SourceLocation() {}
            SourceLocation(const Iterator& first, const Iterator& last);
            
            Iterator first;
            Iterator last;
        };
        
        class Line {
            friend class Parser;
        public:
            typedef boost::shared_ptr<Line> Ptr;
            
            Line(const SourceLocation& loc);
            Ides::String GetText() const;
            const SourceLocation& GetSourceLocation() const { return this->loc; }
        private:
            SourceLocation loc;
        };
        
        class Parser {
        public:
            
            Parser(const Ides::String& src);
            ~Parser();
            
            void* GetScanner() const { return this->scanner; }
            
            int ReadInput(char *buffer, size_t* numBytesRead, int maxBytesToRead);
            
            const Line::Ptr GetCurrentLine() const { return this->current_line; }
            const Ides::String& GetSource() const { return this->src; }
        protected:
            void InitParser();
            void DestroyParser();
        private:
            Line::Ptr current_line;
            void* scanner;
            Ides::String src;
            Iterator src_iter;
            Iterator src_end;
        };
        
        
    }
}

std::ostream& operator<<(std::ostream& os, const Ides::Parsing::SourceLocation& loc);


#include <ides/AST/AST.h>
#include <ides/AST/ASTExpression.h>
#include <ides/AST/ASTConstantExpression.h>


#endif // _IDES_PARSER_H_
