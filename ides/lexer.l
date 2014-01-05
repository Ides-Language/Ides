%top{
    #include <string>
    #include <sstream>
    #include <iostream>
    #include <ides/Lang.h>
    #include <ides/Parsing/AST.h>
    #include <ides/parser.hpp>

#define YY_USER_ACTION { MSG(I_TOK) % yytext % yylineno; \
yylloc->first_line = yylineno; \
yylloc->first_column = yylloc->last_column; \
yylloc->last_column=yylloc->first_column+yyleng; \
yylloc->last_line = yylineno; \
}

}

%option reentrant
%option stack
%option noyywrap
%option yylineno

%option bison-bridge
%option bison-locations

/* %option debug */

%s IN_BLOCKCOMMENT
%s IN_SINGLELINECOMMENT
%x IN_STRING

OP_START [!\#\%\^&\*\-\+/\\<>\|?]
OP {OP_START}[=:{OP_START}]*
ID [A-Za-z_][A-Za-z0-9]*

%%

[ \t\v\r\f\n]           ; // Ignore whitespace, but advance the location

<INITIAL>{

[0-9]+\.[0-9]+          { yylval->ast_dec = new Ides::ConstantDec(strtod(yytext, NULL)); return TDOUBLE; }

0                       { yylval->ast_int = new Ides::ConstantInt(0); return TINTEGER; }
[1-9][0-9]*             { yylval->ast_int = new Ides::ConstantInt(strtol(yytext, NULL, 10)); return TINTEGER; }
0[0-9]+                 { yylval->ast_int = new Ides::ConstantInt(strtol(&yytext[1], NULL, 8)); return TINTEGER; }
0x[0-9A-F]+             { yylval->ast_int = new Ides::ConstantInt(strtol(&yytext[3], NULL, 16)); return TINTEGER; }
0b[0-1]+                { yylval->ast_int = new Ides::ConstantInt(strtol(&yytext[3], NULL, 2)); return TINTEGER; }

"true"                  { yylval->ast_bool = new Ides::ConstantBool(true); return TBOOL; }
"false"                 { yylval->ast_bool = new Ides::ConstantBool(false); return TBOOL; }


"def"                   return KW_DEF;
"fn"                    return KW_FN;
"function"              return KW_FN;
"var"                   return KW_VAR;
"val"                   return KW_VAL;
"struct"                return KW_STRUCT;
"class"                 return KW_CLASS;
"trait"                 return KW_TRAIT;

"null"                  return KW_NULL;
"namespace"             return KW_NAMESPACE;
"case"                  return KW_CASE;

"if"                    return KW_IF;
"else"                  return KW_ELSE;

"public"                return KW_PUBLIC;
"protected"             return KW_PROTECTED;
"internal"              return KW_INTERNAL;
"private"               return KW_PRIVATE;
"extern"                return KW_EXTERN;
"const"                 return KW_CONST;
"abstract"              return KW_ABSTRACT;

"throw"                 return KW_THROW;
"new"                   return KW_NEW;
"return"                return KW_RETURN;
"match"                 return KW_MATCH;

"\.\.\."                return KW_VARARGS;
"=>"                    return KW_DEFINEDAS;

"/*"           BEGIN(IN_BLOCKCOMMENT);
"//"           BEGIN(IN_SINGLELINECOMMENT);

"\""                    { yylval->ast_str = new Ides::ConstantString(""); BEGIN(IN_STRING); }

:[0-9]+                 { yylval->ast_placeholder = new Ides::PlaceholderExpr(strtol(&yytext[1], NULL, 10)); return TPLACEHOLDER; }

\'.\'                   { return TCHAR; }

{OP}                    { yylval->ast_ident = new Ides::IdentifierExpr(yytext); return TOPERATOR; }
{ID}(_{OP})?            { yylval->ast_ident = new Ides::IdentifierExpr(yytext); return TIDENTIFIER; }
`[^`]+`                 { yylval->ast_ident = new Ides::IdentifierExpr(std::string(&yytext[1], strlen(yytext) - 2)); return TIDENTIFIER; }

}

<IN_BLOCKCOMMENT>{
    "*/"                BEGIN(INITIAL);
    [^*\n]+             // eat comment in chunks
    "*"                 // eat the lone star
}

<IN_SINGLELINECOMMENT>{
    [^\n]*              BEGIN(INITIAL); // Eat the rest of the line and continue.
}

<IN_STRING>{
    [^\\"]              { yylval->ast_str->value += yytext; }
    \\a                 { yylval->ast_str->value += '\a'; }
    \\b                 { yylval->ast_str->value += '\b'; }
    \\f                 { yylval->ast_str->value += '\f'; }
    \\n                 { yylval->ast_str->value += '\n'; }
    \\r                 { yylval->ast_str->value += '\r'; }
    \\t                 { yylval->ast_str->value += '\t'; }
    \\v                 { yylval->ast_str->value += '\v'; }
    \\\'                { yylval->ast_str->value += '\''; }
    \\\"                { yylval->ast_str->value += '"'; }

    \\.                 { MSG(E_BADESC) % yytext; yyterminate(); }
    <<EOF>>             { MSG(E_NOENDQUOT); yyterminate(); }
    
    \"                  { BEGIN(INITIAL); return TSTRING; }
}

<<EOF>>                 { yyterminate(); }

.                       return yytext[0];

%%

