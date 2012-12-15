//
//  ParserCommon.h
//  ides
//
//  Created by Sean Edwards on 12/13/12.
//
//

#ifndef _IDES_PARSER_COMMON_H_
#define _IDES_PARSER_COMMON_H_

extern "C" {
#include <stdint.h>
}

#include <ides/Parsing/Parser.h>

#define YYLTYPE Ides::Parsing::SourceLocation
#define YY_EXTRA_TYPE Ides::Parsing::Parser*


#define YY_USER_INIT LOCATION_STEP(*yylloc)


#define YYLLOC_DEFAULT(Cur, Rhs, N)                       \
    do {                                                   \
    if (N) { (Cur).first   = YYRHSLOC(Rhs, 1).first; (Cur).last    = YYRHSLOC(Rhs, N).last; } \
    else { (Cur).first = (Cur).last = YYRHSLOC(Rhs, 0).last; (Cur).first = (Cur).last = YYRHSLOC(Rhs, 0).last;  } \
    } while (0)



#define LOCATION_STEP(Loc) ((Loc).first = (Loc).last)

#endif
