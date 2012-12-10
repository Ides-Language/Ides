#ifndef _IDES_PARSER_H_
#define _IDES_PARSER_H_

#include <string>

#include <ides/AST/AST.h>

namespace Ides {
    namespace Parsing {
        
        
        
        Ides::AST::AST::Ptr Parse(std::istream& is, const std::string& srcname);
        
    }
}


#endif // _IDES_PARSER_H_
