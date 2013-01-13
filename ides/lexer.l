%top{
    #include <string>
    #include <sstream>
    #include <ides/Parsing/ParserCommon.h>
    #include <iostream>
    
    #include <parser.hpp>
    
    #define YY_INPUT(buf, result, maxSize) yyextra->ReadInput(buf, &(result), maxSize)
    
    #define SAVE_TOKEN(x) return x
    
    #define SET_YYLLOC \
    do { \
        yylloc->setBegin(yylloc->getEnd()); \
        yylloc->setEnd(yylloc->getBegin().getLocWithOffset(strlen(yytext))); \
    } while(0)
    
    #define YY_USER_ACTION SET_YYLLOC;
    
    #define strbuf yylval->ast_str->GetBuffer()
    
    
}

%option reentrant
%option stack
%option noyywrap

%option bison-bridge
%option bison-locations

%option debug

%s IN_BLOCKCOMMENT
%s IN_SINGLELINECOMMENT
%x IN_STRING
%x IN_CSTRING
%x IN_WCSTRING
%x IN_LCSTRING
%%

[ \t\v\r\f\n]           ; // Ignore whitespace, but advance the location


[0-9]+\.[0-9]*          { yylval->ast_base = new Ides::AST::ConstantFloatExpression(strtod(yytext, NULL)); return TDOUBLE; }

0                       { yylval->ast_base = new Ides::AST::ConstantIntExpression(0); return TINTEGER; }
[1-9][0-9]*             { yylval->ast_base = new Ides::AST::ConstantIntExpression(strtol(yytext, NULL, 10)); return TINTEGER; }
0[0-9]+                 { yylval->ast_base = new Ides::AST::ConstantIntExpression(strtol(yytext, NULL, 8)); return TINTEGER; }
0x[0-9A-F]+             { yylval->ast_base = new Ides::AST::ConstantIntExpression(strtol(&yytext[3], NULL, 16)); return TINTEGER; }
0b[0-1]+                { yylval->ast_base = new Ides::AST::ConstantIntExpression(strtol(&yytext[3], NULL, 2)); return TINTEGER; }


"def"                   return KW_DEF;
"fn"                    return KW_FN;
"function"              return KW_FN;
"var"                   return KW_VAR;
"val"                   return KW_VAL;
"struct"                return KW_STRUCT;
"class"                 return KW_CLASS;

"null"                  return KW_NULL;
"namespace"             return KW_NAMESPACE;

"public"                return KW_PUBLIC;
"protected"             return KW_PROTECTED;
"internal"              return KW_INTERNAL;
"provate"               return KW_PRIVATE;
"extern"                return KW_EXTERN;
"const"                 return KW_CONST;

"throw"                 return KW_THROW;
"new"                   return KW_NEW;
"return"                return KW_RETURN;

"if"                    return KW_IF;
"else"                  return KW_ELSE;
"while"                 return KW_WHILE;
"for"                   return KW_FOR;
"do"                    return KW_DO;

"void"                  return KW_VOID;
"unit"                  return KW_UNIT;
"bool"                  return KW_BOOL;
"int8"                  return KW_INT8;
"uint8"                 return KW_UINT8;
"int16"                 return KW_INT16;
"uint16"                return KW_UINT16;
"int32"                 return KW_INT32;
"uint32"                return KW_UINT32;
"int64"                 return KW_INT64;
"uint64"                return KW_UINT64;

"float32"               return KW_FLOAT32;
"float64"               return KW_FLOAT64;

"true"                  yylval->ast_base = new Ides::AST::ConstantBoolExpression(true); return KW_TRUE;
"false"                 yylval->ast_base = new Ides::AST::ConstantBoolExpression(false); return KW_FALSE;

"or"                    return OP_COALESCE;
"as"                    return OP_CAST;
"++"                    return OP_INC;
"--"                    return OP_DEC;

"+"                     return OP_PLUS;
"-"                     return OP_MINUS;
"*"                     return OP_STAR;
"/"                     return OP_SLASH;
"%"                     return OP_MOD;

"=="                    return OP_EQ;
"!="                    return OP_NE;
"<"                     return OP_LT;
">"                     return OP_GT;
"<="                    return OP_LE;
">="                    return OP_GE;

"&&"                    return OP_AND;
"||"                    return OP_OR;
"!"                     return OP_NOT;

"->"                    return OP_RARROW;
"<-"                    return OP_LARROW;

"|"                     return OP_BOR;
"&"                     return OP_BAND;
"^"                     return OP_BXOR;
"~"                     return OP_BNOT;

"<<"                    return OP_LSHL;
">>"                    return OP_LSHR;
">>>"                   return OP_ASHL;
"<<<"                   return OP_ASHR;

"="                     return OP_ASSIGN;

"\.\.\."             return KW_VARARGS;



<INITIAL>"/*"           BEGIN(IN_BLOCKCOMMENT);
<INITIAL>"//"           BEGIN(IN_SINGLELINECOMMENT);
<INITIAL>"\""           { yylval->ast_str = new Ides::AST::ConstantStringExpression(); BEGIN(IN_STRING); }
<INITIAL>"C\""          { yylval->ast_str = new Ides::AST::ConstantCStringExpression(); BEGIN(IN_CSTRING); }
<INITIAL>"WC\""         { yylval->ast_str = new Ides::AST::ConstantWCStringExpression(); BEGIN(IN_WCSTRING); }
<INITIAL>"LC\""         { yylval->ast_str = new Ides::AST::ConstantLCStringExpression(); BEGIN(IN_LCSTRING); }

<IN_BLOCKCOMMENT>{
    "*/"                BEGIN(INITIAL);
    [^*\n]+             // eat comment in chunks
    "*"                 // eat the lone star
}

<IN_SINGLELINECOMMENT>{
    [^\n]*              BEGIN(INITIAL); // Eat the rest of the line and continue.
}

<IN_STRING,IN_CSTRING,IN_WCSTRING,IN_LCSTRING>{
    [^\\"]              { strbuf << yytext; }
    \\a                 { strbuf << '\a'; }
    \\b                 { strbuf << '\b'; }
    \\f                 { strbuf << '\f'; }
    \\n                 { strbuf << '\n'; }
    \\r                 { strbuf << '\r'; }
    \\t                 { strbuf << '\t'; }
    \\v                 { strbuf << '\v'; }
    \\\'                { strbuf << '\''; }
    \\\"                { strbuf << '"'; }
    \\.                 { std::cerr << "invalid escape sequence" << std::endl; yyterminate(); }
    <<EOF>>             { std::cerr << "unterminated quote" << std::endl; yyterminate(); }
}

<IN_STRING>\"           { BEGIN(INITIAL); yylval->ast_str->exprloc.setEnd(yylloc->getBegin()); return TSTRING; }
<IN_CSTRING>\"          { BEGIN(INITIAL); yylval->ast_str->exprloc.setEnd(yylloc->getBegin()); return TSTRING; }
<IN_WCSTRING>\"         { BEGIN(INITIAL); yylval->ast_str->exprloc.setEnd(yylloc->getBegin()); return TSTRING; }
<IN_LCSTRING>\"         { BEGIN(INITIAL); yylval->ast_str->exprloc.setEnd(yylloc->getBegin()); return TSTRING; }


\'.\'                   { yylval->ast_base = new Ides::AST::ConstantIntExpression(yytext[1]); return TCHAR; }

[a-zA-Z_][a-zA-Z0-9_]*  { yylval->ast_ident = new Ides::AST::Token(yytext); yylval->ast_ident->exprloc = *yylloc; return TIDENTIFIER; }

<<EOF>>                 { yyterminate(); }

.                       return yytext[0];

%%


void Ides::Parsing::ParseContext::InitParser() {
    yylex_init(&this->scanner);
    yyset_extra(this, this->scanner);
}

void Ides::Parsing::ParseContext::DestroyParser() {
    yylex_destroy(this->scanner);
}
