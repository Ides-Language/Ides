%{
    #include <iostream>
    #include <ides/Parsing/ParserCommon.h>
    
    void yyerror(YYLTYPE* locp, Ides::Parsing::Parser* context, const char* err)
	{
		std::cout << err << std::endl;
	}
    
    int yylex(YYSTYPE* lvalp, YYLTYPE* llocp, void* scanner);
    
    #define scanner context->GetScanner()
%}

%pure-parser
%defines
%error-verbose
%locations

%parse-param { Ides::Parsing::Parser* context }
%lex-param { void* scanner  }

%union {
    std::string *string;
    Ides::Parsing::Token* token;
}

/* Define our terminal symbols (tokens). This should
 match our tokens.l lex file. We also define the node type
 they represent.
 */
%token <char> TCHAR
%token <strval> TIDENTIFIER
%token <strval> TSTRING
%token <intval> TINTEGER
%token <fltval> TDOUBLE

// assignment operators
%token <token> TCEQ TDEFAS
// logical operators
%token <token> TCNE TCLT TCLE TCGT TCGE TEQUAL TEXPT
// bitwise operators
%token <token> TCARET TAMP TBAR TTILDE

// Misc. operators
%token <token> TPOUND TAT

%token <token> TLPAREN TRPAREN TLBRACE TRBRACE TLBRACKET TRBRACKET
%token <token> TCOMMA TDOT TCOLON TSEMICOLON

// Arithmetic operators
%token <token> TPLUS TMINUS TMUL TDIV TMOD

/* Define the type of node our nonterminal symbols represent.
 The types refer to the %union declaration above. Ex: when
 we call an ident (defined by union type ident) we are really
 calling an (NIdentifier*). It makes the compiler happy.
 */
%type <block> program

/* Operator precedence for mathematical operators */
%left TPLUS TMINUS
%left TMUL TDIV

%start program

%%

program : ;

%%
