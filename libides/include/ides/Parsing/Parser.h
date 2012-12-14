#ifndef _IDES_PARSER_H_
#define _IDES_PARSER_H_

#include <string>

#include <boost/shared_ptr.hpp>

#include <ides/AST/AST.h>

namespace Ides {
    namespace Parsing {
        typedef std::string::const_iterator Iterator;
        
        Ides::AST::AST::Ptr Parse(std::istream& is, const std::string& srcname);
        
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
            std::string GetText() const;
            const SourceLocation& GetSourceLocation() const { return this->loc; }
        private:
            SourceLocation loc;
        };
        
        
        class Token {
        public:
            typedef boost::shared_ptr<Token> Ptr;
            
            Token(int token, const SourceLocation& loc, Line::Ptr line);
            static Token::Ptr Create(int token, const SourceLocation& loc, Line::Ptr line);
            
            Line::Ptr GetLine() const { return this->line; }
            std::string GetText() const;
            int GetToken() const { return this->token; }
            const SourceLocation& GetLocation() const { return this->loc; }
            
            union {
                int intval;
                double fltval;
                char* strval;
                char chrval;
            };
        private:
            Line::Ptr line;
            SourceLocation loc;
            int token;
        };
        
        class Parser {
        public:
            
            Parser(const std::string& src);
            void* GetScanner() const { return this->scanner; }
            
            int ReadInput(int max_bytes);
            
            const Line::Ptr GetCurrentLine() const { return this->current_line; }
            const std::string& GetSource() const { return this->src; }
        protected:
            void InitParser();
            void DestroyParser();
        private:
            Line::Ptr current_line;
            void* scanner;
            std::string src;
            Iterator src_iter;
            Iterator src_end;
        };
        
        
    }
}


#endif // _IDES_PARSER_H_
