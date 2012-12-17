%top{
    #include <string>
    #include <sstream>
    #include <ides/Parsing/ParserCommon.h>
    
    #include <ides/AST/ASTConstantExpression.h>
    
    #include <parser.hpp>
    
    #define YY_INPUT(buf, result, maxSize) do { \
        if (yylloc->line_start == Ides::SourceIterator()) \
            yylloc->line_start = yylloc->first = yyextra->GetSourceIterator(); \
        yyextra->ReadInput(buf, &(result), maxSize); \
    } while(0)
    
    #define SAVE_TOKEN(x) return x
    
    #define SET_YYLSTART (*yylloc).first = yyextra->GetSourceIterator()
    #define SET_YYLEND yylloc->last = (yylloc->line_end = yyextra->GetSourceIterator()) - 1
    #define SET_YYLLOC do { SET_YYLEND; yylloc->first = yylloc->last - strlen(yytext); } while(0)
    
    static std::stringstream strbuf;
    
}

%option reentrant
%option stack
%option noyywrap
%option yylineno

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

[ \t\v\r\f]             ; // Ignore whitespace.

[\n]                    { SET_YYLLOC; yylloc->line_start = yylloc->last + 1; ++(*yylloc).line_num; }


[0-9]+\.[0-9]*          { SET_YYLLOC; double val = atof(yytext); return TDOUBLE; }

0                       { SET_YYLLOC; yylval->ast_base = new Ides::AST::ASTConstantIntExpression(0); return TINTEGER; }
[1-9][0-9]*             { SET_YYLLOC; yylval->ast_base = new Ides::AST::ASTConstantIntExpression(strtol(yytext, NULL, 10)); return TINTEGER; }
0[0-9]+                 { SET_YYLLOC; yylval->ast_base = new Ides::AST::ASTConstantIntExpression(strtol(yytext, NULL, 8)); return TINTEGER; }
0x[0-9A-F]+             { SET_YYLLOC; yylval->ast_base = new Ides::AST::ASTConstantIntExpression(strtol(&yytext[3], NULL, 16)); return TINTEGER; }
0b[0-1]+                { SET_YYLLOC; yylval->ast_base = new Ides::AST::ASTConstantIntExpression(strtol(&yytext[3], NULL, 2)); return TINTEGER; }


"def"                   SET_YYLLOC; return KW_DEF;
"var"                   SET_YYLLOC; return KW_VAR;
"val"                   SET_YYLLOC; return KW_VAL;
"throw"                 SET_YYLLOC; return KW_THROW;
"new"                   SET_YYLLOC; return KW_NEW;
"extern"                SET_YYLLOC; return KW_EXTERN;
"null"                  SET_YYLLOC; return KW_NULL;
"return"                SET_YYLLOC; return KW_RETURN;

"int8"                  SET_YYLLOC; return KW_INT8;
"int16"                 SET_YYLLOC; return KW_INT16;
"int32"                 SET_YYLLOC; return KW_INT32;
"int64"                 SET_YYLLOC; return KW_INT64;

"or"                    SET_YYLLOC; return OP_COALESCE;
"as"                    SET_YYLLOC; return OP_CAST;
"++"                    SET_YYLLOC; return OP_INC;
"--"                    SET_YYLLOC; return OP_DEC;
"=="                    SET_YYLLOC; return OP_EQ;


<INITIAL>"/*"           SET_YYLSTART; BEGIN(IN_BLOCKCOMMENT);
<INITIAL>"//"           SET_YYLSTART; BEGIN(IN_SINGLELINECOMMENT);
<INITIAL>"\""           SET_YYLSTART; BEGIN(IN_STRING);
<INITIAL>"C\""          SET_YYLSTART; BEGIN(IN_CSTRING);
<INITIAL>"WC\""         SET_YYLSTART; BEGIN(IN_WCSTRING);
<INITIAL>"LC\""         SET_YYLSTART; BEGIN(IN_LCSTRING);

<IN_BLOCKCOMMENT>{
    "*/"                BEGIN(INITIAL);
    [^*\n]+             // eat comment in chunks
    "*"                 // eat the lone star
}

<IN_SINGLELINECOMMENT>{
    [^\n]*              BEGIN(INITIAL); // Eat the rest of the line and continue.
}

<IN_STRING,IN_CSTRING,IN_WCSTRING,IN_LCSTRING>{
    [^\\"]            { SET_YYLEND; strbuf << yytext; }
    \\a                 { SET_YYLEND; strbuf << '\a'; }
    \\b                 { SET_YYLEND; strbuf << '\b'; }
    \\f                 { SET_YYLEND; strbuf << '\f'; }
    \\n                 { SET_YYLEND; strbuf << '\n'; }
    \\r                 { SET_YYLEND; strbuf << '\r'; }
    \\t                 { SET_YYLEND; strbuf << '\t'; }
    \\v                 { SET_YYLEND; strbuf << '\v'; }
    \\\'                { SET_YYLEND; strbuf << '\''; }
    \\\"                { SET_YYLEND; strbuf << '"'; }
    \\.                 { SET_YYLEND; std::cerr << "invalid escape sequence" << std::endl; yyterminate(); }
    <<EOF>>             { SET_YYLEND; std::cerr << "unterminated quote" << std::endl; yyterminate(); }
}

<IN_STRING>\"           { SET_YYLEND; BEGIN(INITIAL); yylval->ast_istr = new Ides::AST::ASTConstantStringExpression(strbuf.str()); strbuf.str(""); return TSTRING; }
<IN_CSTRING>\"          { SET_YYLEND; BEGIN(INITIAL); yylval->ast_cstr = new Ides::AST::ASTConstantCStringExpression(strbuf.str()); strbuf.str(""); return TSTRING; }
<IN_WCSTRING>\"         { SET_YYLEND; BEGIN(INITIAL); yylval->ast_wstr = new Ides::AST::ASTConstantWCStringExpression(strbuf.str()); strbuf.str(""); return TSTRING; }
<IN_LCSTRING>\"         { SET_YYLEND; BEGIN(INITIAL); yylval->ast_lstr = new Ides::AST::ASTConstantLCStringExpression(strbuf.str()); strbuf.str(""); return TSTRING; }


\'.\'                   { SET_YYLLOC; yylval->ast_base = new Ides::AST::ASTConstantCharExpression(yytext[1]); return TCHAR; }

[a-zA-Z_][a-zA-Z0-9_]*  { SET_YYLLOC; yylval->ast_ident = new Ides::AST::ASTIdentifier(yytext); return TIDENTIFIER; }

<<EOF>>                 { yyterminate(); }

.                       (*yylloc).first = (*yylloc).last = yyextra->GetSourceIterator(); return yytext[0];

%%


void Ides::Parsing::Parser::InitParser() {
    yylex_init(&this->scanner);
    yyset_extra(this, this->scanner);
}

void Ides::Parsing::Parser::DestroyParser() {
    yylex_destroy(this->scanner);
}
