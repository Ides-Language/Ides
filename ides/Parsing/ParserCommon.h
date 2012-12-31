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

#include <ides/Parsing/ParseContext.h>

#include <ides/AST/AST.h>
#include <ides/AST/Expression.h>
#include <ides/AST/Declaration.h>
#include <ides/AST/Statement.h>
#include <ides/AST/ConstantExpression.h>

#include <ides/Types/Type.h>

#include <clang/Basic/SourceLocation.h>

#define YYLTYPE clang::SourceRange
#define YY_EXTRA_TYPE Ides::Parsing::ParseContext*

#define YY_TYPEDEF_YY_SIZE_T
typedef int yy_size_t;


#define YY_USER_INIT *yylloc = yyextra->GetFileStartLocation();

#define YYLLOC_DEFAULT(Current, Rhs, N)          \
    Current = clang::SourceRange(Rhs[1].getBegin(), Rhs[N].getEnd());



//#define LOCATION_STEP(Loc) ((Loc).first = (Loc).last)

#endif
