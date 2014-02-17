%top{
    #include <string>
    #include <sstream>
    #include <iostream>
    #include <ides/Lang.h>
    #include <ides/Parsing/AST.h>
    #include <ides/Source/SourceLocation.h>
    #include <ides/parser.hpp>
    #include <boost/variant/get.hpp>

#define YYLTYPE Ides::SourceRange

#define YY_USER_ACTION { \
*yylloc = Ides::SourceRange(yylloc->begin + yylloc->length, (size_t)yyleng); \
MSG(I_TOK) % yytext;\
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

OP_START [!\#\%\^&\*\-\+/\\<>\|?~]
OP {OP_START}[=:!\#\%\^&\*\-\+/\\<>\|?~]*
ID [A-Za-z_][A-Za-z0-9]*

%%

[ \t\v\r\f\n]           ; // Ignore whitespace, but advance the location

<INITIAL>{

[0-9]+\.[0-9]+          {
    yylval->ast_dec = new Ides::ConstantDec(strtod(yytext, NULL));
    yylval->ast->source = *yylloc;
    return TDOUBLE;
}

0                       {
    yylval->ast_int = new Ides::ConstantInt((uint64_t)0);
    yylval->ast->source = *yylloc;
    return TINTEGER;
}

[1-9][0-9]*             {
    yylval->ast_int = new Ides::ConstantInt((uint64_t)strtol(yytext, NULL, 10));
    yylval->ast->source = *yylloc;
    return TINTEGER;
}

0[0-9]+                 {
    yylval->ast_int = new Ides::ConstantInt((uint64_t)strtol(&yytext[1], NULL, 8));
    yylval->ast->source = *yylloc;
    return TINTEGER;
}

0x[0-9A-F]+             {
    yylval->ast_int = new Ides::ConstantInt((uint64_t)strtol(&yytext[3], NULL, 16));
    yylval->ast->source = *yylloc;
    return TINTEGER;
}

0b[0-1]+                {
    yylval->ast_int = new Ides::ConstantInt((uint64_t)strtol(&yytext[3], NULL, 2));
    yylval->ast->source = *yylloc;
    return TINTEGER;
}

"true"                  { yylval->ast_bool = new Ides::ConstantBool((bool)true); yylval->ast->source = *yylloc; return TBOOL; }
"false"                 { yylval->ast_bool = new Ides::ConstantBool((bool)false); yylval->ast->source = *yylloc; return TBOOL; }


"def"                   return KW_DEF;
"fn"                    return KW_FN;
"function"              return KW_FN;
"var"                   return KW_VAR;
"val"                   return KW_VAL;
"struct"                return KW_STRUCT;
"class"                 return KW_CLASS;
"trait"                 return KW_TRAIT;
"mod"                   return KW_MOD;

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
"unsafe"                return KW_UNSAFE;
"intrinsic"             return KW_INTRINSIC;
"implicit"              return KW_IMPLICIT;
"locked"                return KW_LOCKED;

"throw"                 return KW_THROW;
"new"                   return KW_NEW;
"return"                return KW_RETURN;
"match"                 return KW_MATCH;
"as"                    return KW_AS;

"\.\.\."                return KW_VARARGS;
"\.\."                  return KW_RANGE;
"=>"                    return KW_DEFINEDAS;

"/*"           BEGIN(IN_BLOCKCOMMENT);
"//"           BEGIN(IN_SINGLELINECOMMENT);

"\""                    {
    yylval->ast_str = new Ides::ConstantString(std::string(""));
    yylval->ast->source = *yylloc;
    BEGIN(IN_STRING);
}

:[0-9]+                 {
    yylval->ast_placeholder = new Ides::PlaceholderExpr((Ides::placeholder_t)strtol(&yytext[1], NULL, 10));
    yylval->ast->source = *yylloc;
    return TPLACEHOLDER;
}

\'.\'                   {
    yylval->ast_int = new Ides::ConstantChar((char)*yytext);
    yylval->ast->source = *yylloc;
    return TCHAR;
}

{OP}                    {
    yylval->ast_ident = new Ides::IdentifierExpr(yytext);
    yylval->ast->source = *yylloc;
    return TOPERATOR;
}

{ID}(_{OP})?            {
    yylval->ast_ident = new Ides::IdentifierExpr(yytext);
    yylval->ast->source = *yylloc;
    return TIDENTIFIER;
}

`[^`]+`                 {
    yylval->ast_ident = new Ides::IdentifierExpr(std::string(&yytext[1], strlen(yytext) - 2));
    yylval->ast->source = *yylloc;
    return TIDENTIFIER;
}

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
    [^\\"]              { boost::get<Ides::String>(yylval->ast_str->value) += yytext; }
    \\a                 { boost::get<Ides::String>(yylval->ast_str->value) += '\a'; }
    \\b                 { boost::get<Ides::String>(yylval->ast_str->value) += '\b'; }
    \\f                 { boost::get<Ides::String>(yylval->ast_str->value) += '\f'; }
    \\n                 { boost::get<Ides::String>(yylval->ast_str->value) += '\n'; }
    \\r                 { boost::get<Ides::String>(yylval->ast_str->value) += '\r'; }
    \\t                 { boost::get<Ides::String>(yylval->ast_str->value) += '\t'; }
    \\v                 { boost::get<Ides::String>(yylval->ast_str->value) += '\v'; }
    \\\'                { boost::get<Ides::String>(yylval->ast_str->value) += '\''; }
    \\\"                { boost::get<Ides::String>(yylval->ast_str->value) += '"'; }

    \\.                 { MSG(E_BADESC) % yytext; yyterminate(); }
    <<EOF>>             { MSG(E_NOENDQUOT); yyterminate(); }
    
    \"                  { BEGIN(INITIAL); return TSTRING; }
}

<<EOF>>                 { yyterminate(); }

.                       return yytext[0];

%%

