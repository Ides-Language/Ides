%{
    #include <iostream>
    #include <ides/Parsing/Parser.h>
    #include <ides/lexer.hpp>
    #include <stdexcept>
    #include <sstream>

    void yyerror(YYLTYPE* locp, Ides::Parser* context, Ides::Ast** output, const char* err)
	{
        throw std::runtime_error(err);
	}

    #define scanner context->GetScanner()


%}

%pure-parser
%defines
%error-verbose
%locations
%debug

%parse-param { Ides::Parser* context }
%parse-param { Ides::Ast** output }

%lex-param { yyscan_t scanner }

%union {
    Ides::Ast* ast;
    Ides::Expr* expr;
    Ides::Decl* decl;
    Ides::DataStructureDecl* decl_type;

    Ides::TupleExpr* tuple_expr;

    Ides::ConstantInt* ast_int;
    Ides::ConstantString* ast_str;
    Ides::ConstantChar* ast_char;
    Ides::ConstantDec* ast_dec;
    Ides::ConstantBool* ast_bool;
    Ides::IdentifierExpr* ast_ident;

    Ides::ValKind val_kind;
    Ides::DataKind data_kind;
    Ides::Visibility ident_vis;
}

// Builtin types
%token KW_VOID KW_UNIT KW_BOOL KW_INT8 KW_UINT8 KW_INT16 KW_UINT16 KW_INT32 KW_UINT32 KW_INT64 KW_UINT64 KW_FLOAT32 KW_FLOAT64

// Specifiers
%token KW_PUBLIC KW_PROTECTED KW_INTERNAL KW_PRIVATE KW_EXTERN KW_CONST KW_ABSTRACT

// Keywords
%token KW_DEF KW_FN KW_STRUCT KW_CLASS KW_TRAIT KW_VAR KW_VAL KW_TYPE KW_NULL KW_NAMESPACE
%token KW_IF KW_ELSE KW_DO KW_WHILE KW_FOR
%token KW_VARARGS
// Keyword operators
%token KW_THROW KW_NEW KW_RETURN

%token <ast_int> TINTEGER
%token <ast_ident> TIDENTIFIER
%token <ast_ident> TOPERATOR
%token <ast_char> TCHAR
%token <ast_str> TSTRING
%token <ast_dec> TDOUBLE
%token <ast_bool> TBOOL

%type <ast> root program
%type <ast_ident> infix_operator ident
%type <expr> constant unit_expr primary_expr prefix_expr postfix_expr infix_expr expr stmt
%type <decl> val_decl trait_decl
%type <decl_type> data_decl
%type <tuple_expr> compound_expr index_expr tuple_expr tuple_items

%type <ident_vis> vis vis_const vis_extern vis_abstract vis_type




/* Operator precedence for mathematical operators */
%nonassoc KW_THROW KW_RETURN KW_NEW

%start root

%%

root : program { *output = $$ = $1; }
;

program : compound_expr
;


constant : TBOOL
         | TCHAR
         | TSTRING
         | TDOUBLE
         | TINTEGER
;

infix_operator : TOPERATOR
;

ident : TOPERATOR
      | TIDENTIFIER
;

stmt : expr ';'
     | expr
     | trait_decl
;

compound_expr : compound_expr stmt { $$ = $1; $$->Add($2); }
              | stmt { $$ = new Ides::TupleExpr(); $$->Add($1); }
;


primary_expr : constant
             | TIDENTIFIER
             | val_decl
             | '(' tuple_items ')' { $$ = $2; }
             | '{' compound_expr '}' { $$ = $2; }
;

postfix_expr : postfix_expr tuple_expr { $$ = new Ides::CallExpr($1, $2); }
             | postfix_expr index_expr { $$ = new Ides::IndexExpr($1, $2); }
             | postfix_expr '.' ident { $$ = new Ides::DotExpr($1, $3); }
             | primary_expr
;

prefix_expr : TOPERATOR prefix_expr { $$ = $2; }
            | postfix_expr
;

infix_expr : prefix_expr infix_operator expr { $$ = Ides::InfixExpr::Create($2, $1, $3); }
;

expr : infix_expr
     | prefix_expr
;

unit_expr : constant
          | TIDENTIFIER
;


tuple_items : tuple_items ',' expr { $$ = $1; $$->Add($3); }
            | expr { $$ = new Ides::TupleExpr(); $$->Add($1); }
;

index_expr : '[' tuple_items ']' { $$ = $2; }
           | '[' ']' { $$ = new Ides::TupleExpr(); }
;

tuple_expr : '(' tuple_items ')' { $$ = $2; }
           | '(' ')' { $$ = new Ides::TupleExpr(); }
;

val_decl : vis KW_VAL TIDENTIFIER { $$ = new Ides::ValDecl($1, $3, NULL); }
         | vis KW_VAR TIDENTIFIER { $$ = new Ides::VarDecl($1, $3, NULL); }
         | KW_VAL TIDENTIFIER     { $$ = new Ides::ValDecl(Ides::V_DEFAULT, $2, NULL); }
         | KW_VAR TIDENTIFIER     { $$ = new Ides::VarDecl(Ides::V_DEFAULT, $2, NULL); }
;

vis : vis_const;

vis_const : vis_extern
          /*| vis_extern KW_CONST { $$ = (Ides::Visibility)($1 | Ides::V_CONST); }*/
;


vis_extern : vis_abstract
           | vis_abstract KW_EXTERN { $$ = (Ides::Visibility)($1 | Ides::V_EXTERN); }
;

vis_abstract : vis_type
             | vis_type KW_ABSTRACT { $$ = (Ides::Visibility)($1 | Ides::V_ABSTRACT); }
;

vis_type : KW_PUBLIC { $$ = Ides::V_PUBLIC; }
         | KW_PRIVATE { $$ = Ides::V_PRIVATE; }
         | KW_PROTECTED { $$ = Ides::V_PROTECTED; }
         | KW_INTERNAL { $$ = Ides::V_INTERNAL; }
;

trait_decl : vis KW_TRAIT TIDENTIFIER data_decl { $$ = new Ides::TraitDecl($1, $3, $4); }
           | KW_TRAIT TIDENTIFIER data_decl { $$ = new Ides::TraitDecl(Ides::V_DEFAULT, $2, $3); }
;

data_decl : ':' tuple_items '{' compound_expr '}' { $$ = new Ides::DataStructureDecl($2, $4); }
          | ':' tuple_items '{' '}' { $$ = new Ides::DataStructureDecl($2, new Ides::TupleExpr()); }
          | '{' compound_expr '}' { $$ = new Ides::DataStructureDecl(new Ides::TupleExpr(), $2); }
          | '{' '}'{ $$ = new Ides::DataStructureDecl(new Ides::TupleExpr(), new Ides::TupleExpr()); }
;

%%
