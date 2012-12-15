%top{
    #include <string>
    #include <sstream>
    #include <ides/Parsing/ParserCommon.h>
    
    #include <ides/AST/ASTConstantExpression.h>
    
    #include <parser.hpp>
    
    #define YY_INPUT(buf, result, maxSize) yyextra->ReadInput(buf, &(result), maxSize)
    
    #define SAVE_TOKEN(x) return x
    
    static std::stringstream strbuf;
    
    typedef Ides::AST::ASTExpression* (*MakeString)();
    
    Ides::AST::ASTExpression* MakeIdesString() { return (Ides::AST::ASTExpression*)new Ides::AST::ASTConstantStringExpression(strbuf.str()); }
    Ides::AST::ASTExpression* MakeCString() { return (Ides::AST::ASTExpression*)new Ides::AST::ASTConstantCStringExpression(strbuf.str()); }
    Ides::AST::ASTExpression* MakeWCString() { return (Ides::AST::ASTExpression*)new Ides::AST::ASTConstantWCStringExpression(strbuf.str()); }
    Ides::AST::ASTExpression* MakeLCString() { return (Ides::AST::ASTExpression*)new Ides::AST::ASTConstantLCStringExpression(strbuf.str()); }
    
    MakeString stringFactory;
}

%option reentrant
%option stack
%option noyywrap
%option yylineno

%option warn

%option bison-bridge
%option bison-locations

%option debug

%s IN_BLOCKCOMMENT
%s IN_SINGLELINECOMMENT
%x IN_STRING
%%

[ \t\v\r\f]             ; // Ignore whitespace.

[\n]                    {  }

<<EOF>>                 { yyterminate(); }


[0-9]+\.[0-9]*          { double val = atof(yytext); return TDOUBLE; }

0                       { yylval->ast_base = new Ides::AST::ASTConstantIntExpression(0); return TINTEGER; }
[1-9][0-9]*             { yylval->ast_base = new Ides::AST::ASTConstantIntExpression(strtol(yytext, NULL, 10)); return TINTEGER; }
0[0-9]+                 { yylval->ast_base = new Ides::AST::ASTConstantIntExpression(strtol(yytext, NULL, 8)); return TINTEGER; }
0x[0-9A-F]+             { yylval->ast_base = new Ides::AST::ASTConstantIntExpression(strtol(&yytext[3], NULL, 16)); return TINTEGER; }
0b[0-1]+                { yylval->ast_base = new Ides::AST::ASTConstantIntExpression(strtol(&yytext[3], NULL, 2)); return TINTEGER; }


"def"                   return KW_DEF;
"var"                   return KW_VAR;
"val"                   return KW_VAL;
"throw"                 return KW_THROW;
"new"                   return KW_NEW;
"or"                    return KW_OR;
"as"                    return KW_AS;
"extern"                return KW_EXTERN;
"null"                  return KW_NULL;

"++"                    return OP_INC;
"--"                    return OP_DEC;
"=="                    return OP_EQ;


<INITIAL>"/*"           BEGIN(IN_BLOCKCOMMENT);
<INITIAL>"//"           BEGIN(IN_SINGLELINECOMMENT);
<INITIAL>"\""           { stringFactory = &MakeIdesString; BEGIN(IN_STRING); }
<INITIAL>"C\""           { stringFactory = &MakeCString; BEGIN(IN_STRING); }
<INITIAL>"WC\""           { stringFactory = &MakeWCString; BEGIN(IN_STRING); }
<INITIAL>"LC\""           { stringFactory = &MakeLCString; BEGIN(IN_STRING); }

<IN_BLOCKCOMMENT>{
    "*/"                BEGIN(INITIAL);
    [^*\n]+             // eat comment in chunks
    "*"                 // eat the lone star
}

<IN_SINGLELINECOMMENT>{
    [^\n]*              BEGIN(INITIAL); // Eat the rest of the line and continue.
}

<IN_STRING>{
    [^\\"\n]            { strbuf << yytext; }
    \\a                 { strbuf << '\a'; }
    \\b                 { strbuf << '\b'; }
    \\f                 { strbuf << '\f'; }
    \\n                 { strbuf << '\n'; }
    \\r                 { strbuf << '\r'; }
    \\t                 { strbuf << '\t'; }
    \\v                 { strbuf << '\v'; }
    \\\'                 { strbuf << '\''; }
    \\\"                 { strbuf << '"'; }

    \"                  { BEGIN(INITIAL); yylval->ast_base = (*stringFactory)(); strbuf.str(""); return TSTRING; }
}


\'.\'                   { yylval->ast_base = new Ides::AST::ASTConstantCharExpression(yytext[1]); return TCHAR; }

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
