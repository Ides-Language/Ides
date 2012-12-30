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

#include <ides/AST/AST.h>
#include <ides/AST/ASTExpression.h>
#include <ides/AST/ASTConstantExpression.h>

#include <ides/Types/Type.h>

#define YYLTYPE Ides::Diagnostics::SourceLocation
#define YY_EXTRA_TYPE Ides::Parsing::Parser*

#define YY_TYPEDEF_YY_SIZE_T
typedef int yy_size_t;


#define YY_USER_INIT yylloc->line_start = yylloc->first; yylloc->source_name = yyextra->GetSourceName();


#define YYLLOC_DEFAULT(Cur, Rhs, N)                       \
    do {                                                   \
    if (N) { (Cur) = YYRHSLOC(Rhs, 1) + YYRHSLOC(Rhs, N); } \
    else { (Cur) = YYRHSLOC(Rhs, 0);  } \
    } while (0)



//#define LOCATION_STEP(Loc) ((Loc).first = (Loc).last)

#endif
