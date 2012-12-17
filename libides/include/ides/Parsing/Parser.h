#ifndef _IDES_PARSER_H_
#define _IDES_PARSER_H_

#include <string>
#include <ides/common.h>
#include <boost/shared_ptr.hpp>

#include <ides/Diagnostics/SourceLocation.h>

namespace Ides {
    namespace AST {
        class AST;
    }
    
    namespace Parsing {
        
        Ides::AST::AST* Parse(std::istream& is, const Ides::String& srcname);
        
        class Parser {
        public:
            
            Parser(const Ides::String& src, const Ides::String& srcname);
            ~Parser();
            
            void* GetScanner() const { return this->scanner; }
            
            int ReadInput(char *buffer, size_t* numBytesRead, int maxBytesToRead);
            
            const Ides::String& GetSource() const { return this->src; }
            const Ides::String& GetSourceName() const { return this->src_name; }
            
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
        };
        
        
    }
}


#endif // _IDES_PARSER_H_
