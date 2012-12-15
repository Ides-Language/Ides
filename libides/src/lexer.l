%top{
    #include <string>
    #include <sstream>
    #include <ides/Parsing/ParserCommon.h>
    
    #include <ides/AST/ASTConstantExpression.h>
    
    #include <parser.hpp>
    
    #define YY_INPUT(buf, result, maxSize) yyextra->ReadInput(buf, &(result), maxSize)
    
    #define SAVE_TOKEN(x) return x
}

%option reentrant
%option stack
%option noyywrap
%option yylineno

%option warn

%option bison-bridge
%option bison-locations

%option debug

%s IN_COMMENT
%s IN_STRING
%%

[ \t\v\r\f]             ; // Ignore whitespace.

[\n]                    {  }

<<EOF>>                 { yyterminate(); }


[0-9]+\.[0-9]*          { double val = atof(yytext); return TDOUBLE; }

0x[0-9A-F]+             { int64_t val = strtol(&yytext[3], NULL, 16); yylval->ast_base = new Ides::AST::ASTConstantIntExpression(val); return TINTEGER; }
0b[0-1]+                { int64_t val = strtol(&yytext[3], NULL, 2); yylval->ast_base = new Ides::AST::ASTConstantIntExpression(val); return TINTEGER; }
[0-9]+                  { int64_t val = strtol(yytext, NULL, 10); yylval->ast_base = new Ides::AST::ASTConstantIntExpression(val); return TINTEGER; }


"def"                   return KW_DEF;
"var"                   return KW_VAR;
"val"                   return KW_VAL;

"++"                    return OP_INC;
"--"                    return OP_DEC;

"or"                    return KW_OR;


<INITIAL>{
    "/*"                BEGIN(IN_COMMENT);
}
<IN_COMMENT>{
    "*/"                BEGIN(INITIAL);
    [^*\n]+             // eat comment in chunks
    "*"                 // eat the lone star
}

<INITIAL>"\""           { return TSTRING; }
(L|W)?\'.\'             { return TCHAR; }

[a-zA-Z_][a-zA-Z0-9_]*  { yylval->ast_ident = new Ides::AST::ASTIdentifier(yytext); return TIDENTIFIER; }

.                       return yytext[0];

%%


void Ides::Parsing::Parser::InitParser() {
    yylex_init(&this->scanner);
    yyset_extra(this, this->scanner);
}

void Ides::Parsing::Parser::DestroyParser() {
    yylex_destroy(this->scanner);
}
