%top{
    #include <string>
    #include <sstream>
    #include <parser.hpp>
    #include <ides/Parsing/ParserCommon.h>
    
    #define YY_INPUT(buf, result, maxSize) (result = yyextra->ReadInput(maxSize));
}

%{
    std::string parse_string();
    int64_t parse_int(const std::string& str);
    int64_t parse_hexnum(const std::string& str);

    #define SAVE_TOKEN(x) yylval->reset(new Ides::Parsing::Token(x, *yylloc, yyextra->GetCurrentLine()))
    #define RETURN_TOKEN(x) { SAVE_TOKEN(x); return x; }
%}

%option reentrant
%option stack
%option noyywrap
%option yylineno

%option warn

%option bison-bridge
%option bison-locations

%s IN_COMMENT
%s IN_STRING
%%

[ \t\v\r\f]             ; // Ignore whitespace.

[\n]                    {  }

<<EOF>>                 {  }

[a-zA-Z_][a-zA-Z0-9_]*  { SAVE_TOKEN(TIDENTIFIER); return TIDENTIFIER; }


-?[0-9]+\.[0-9]*        { SAVE_TOKEN(TDOUBLE); return TDOUBLE; }
-?0x[0-9A-F]+           { SAVE_TOKEN(TINTEGER); return TINTEGER; }
-?(0[xb]?)[0-9]+        { SAVE_TOKEN(TINTEGER); (*yylval)->intval = parse_int((*yylval)->GetText()); return TINTEGER; }

<INITIAL>{
    "/*"                BEGIN(IN_COMMENT);
}
<IN_COMMENT>{
    "*/"                BEGIN(INITIAL);
    [^*\n]+             // eat comment in chunks
    "*"                 // eat the lone star
}

<INITIAL>"\""                    { parse_string(); return TSTRING; }
(L|W)?\'.\'           { SAVE_TOKEN(TCHAR); }

"="                     { SAVE_TOKEN(TEQUAL); }
"=="                    { SAVE_TOKEN(TCEQ); }
"!="                    { SAVE_TOKEN(TCNE); }
"::="                   { SAVE_TOKEN(TDEFAS); }

"("                     { SAVE_TOKEN(TLPAREN); }
")"                     { SAVE_TOKEN(TRPAREN); }
"{"                     { SAVE_TOKEN(TLBRACE); }
"}"                     { SAVE_TOKEN(TRBRACE); }
"["                     { SAVE_TOKEN(TLBRACKET); }
"]"                     { SAVE_TOKEN(TRBRACKET); }

"."                     { SAVE_TOKEN(TDOT); }
","                     { SAVE_TOKEN(TCOMMA); }

"+"                     { SAVE_TOKEN(TPLUS); }
"-"                     { SAVE_TOKEN(TMINUS); }
"*"                     { SAVE_TOKEN(TMUL); }
"/"                     { SAVE_TOKEN(TDIV); }
"%"                     { SAVE_TOKEN(TMOD); }

"^"                     { SAVE_TOKEN(TCARET); }
"&"                     { SAVE_TOKEN(TAMP); }
"|"                     { SAVE_TOKEN(TBAR); }
"~"                     { SAVE_TOKEN(TTILDE); }
"!"                     { SAVE_TOKEN(TEXPT); }

"<"                     { SAVE_TOKEN(TCLT); }
"<="                    { SAVE_TOKEN(TCLE); }
">"                     { SAVE_TOKEN(TCGT); }
">="                    { SAVE_TOKEN(TCGE); }

":"                     { SAVE_TOKEN(TCOLON); }
";"                     { SAVE_TOKEN(TSEMICOLON); }

"@"                     { SAVE_TOKEN(TAT); }
"#"                     { SAVE_TOKEN(TPOUND); }

.                       printf("Unrecognized token!\n"); yyterminate();

%%


void Ides::Parsing::Parser::InitParser() {
    yylex_init(&this->scanner);
    yyset_extra(this, this->scanner);
}

void Ides::Parsing::Parser::DestroyParser() {
    yylex_destroy(this->scanner);
}


int64_t parse_int(const std::string& str) {
    bool negative = false;
    
}
