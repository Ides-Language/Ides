%{
    #include <iostream>
    #include <ides/Parsing/Parser.h>
    #include <ides/lexer.hpp>
    #include <stdexcept>
    #include <sstream>

    void yyerror(YYLTYPE* locp, Ides::Parser* context, Ides::Ast** output, const char* err)
	{
        MSG(E_PARSE) % err % locp->first_line;
	}

    #define YYLEX_PARAM context->GetScanner()

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
    Ides::PartialFunction* pf_expr;
    Ides::Name* name_expr;

    Ides::ConstantInt* ast_int;
    Ides::ConstantString* ast_str;
    Ides::ConstantChar* ast_char;
    Ides::ConstantDec* ast_dec;
    Ides::ConstantBool* ast_bool;
    Ides::PlaceholderExpr* ast_placeholder;
    Ides::IdentifierExpr* ast_ident;

    Ides::ValKind val_kind;
    Ides::DataKind data_kind;
    Ides::Visibility ident_vis;

    uint8_t placeholder;
}

%initial-action {
    yylloc.first_line = 0;
    yylloc.last_line = 0;
    yylloc.first_column = 0;
    yylloc.last_column = 0;
}

// Builtin types
%token KW_VOID KW_UNIT KW_BOOL KW_INT8 KW_UINT8 KW_INT16 KW_UINT16 KW_INT32 KW_UINT32 KW_INT64 KW_UINT64 KW_FLOAT32 KW_FLOAT64

// Specifiers
%token KW_PUBLIC KW_PROTECTED KW_INTERNAL KW_PRIVATE KW_EXTERN KW_CONST KW_ABSTRACT

// Keywords
%token KW_DEF KW_FN KW_STRUCT KW_CLASS KW_TRAIT KW_VAR KW_VAL KW_TYPE KW_NULL KW_NAMESPACE KW_CASE
%token KW_IF KW_ELSE
%token KW_DEFINEDAS
%token KW_VARARGS
// Keyword operators
%token KW_THROW KW_NEW KW_RETURN KW_MATCH

%token <ast_int> TINTEGER
%token <ast_ident> TIDENTIFIER
%token <ast_ident> TOPERATOR
%token <ast_char> TCHAR
%token <ast_str> TSTRING
%token <ast_dec> TDOUBLE
%token <ast_bool> TBOOL
%token <ast_placeholder> TPLACEHOLDER

%type <ast> root program
%type <ast_ident> operator ident
%type <expr> constant primary_expr prefix_expr postfix_expr infix_expr if_expr else_expr expr stmt
%type <decl> val_decl trait_decl class_decl struct_decl fn_decl
%type <decl_type> data_decl
%type <tuple_expr> compound_expr tuple_items arg_items
%type <pf_expr> pf_items

%type <ident_vis> vis vis_extern vis_const vis_abstract vis_type
%type <name_expr> name




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

operator : TOPERATOR
         | '=' { $$ = new Ides::IdentifierExpr("="); }
         | KW_MATCH { $$ = new Ides::IdentifierExpr("match"); }
;

ident : TOPERATOR
      | TIDENTIFIER
;

stmt_end :
         | ';'
;

stmt : expr stmt_end
     | expr stmt_end
     | fn_decl stmt_end
     | trait_decl stmt_end
     | class_decl stmt_end
     | struct_decl stmt_end
;

name : ident { $$ = new Ides::Name($1); }
     | ident '[' tuple_items ']' { $$ = new Ides::Name($1, $3); }
;

compound_expr : { $$ = new Ides::TupleExpr(); }
              | compound_expr stmt { $$ = $1; $$->Add($2); }
;

primary_expr : constant
             | TIDENTIFIER
             | TPLACEHOLDER
             | val_decl
             | '(' tuple_items ')' { $$ = $2; }
             | '{' compound_expr '}' { $$ = $2; }
             | '{' pf_items '}' { $$ = $2; }
             | else_expr
;

postfix_expr : postfix_expr '(' tuple_items ')' { $$ = new Ides::CallExpr($1, $3); }
             | postfix_expr '[' tuple_items ']' { $$ = new Ides::IndexExpr($1, $3); }
             | postfix_expr '{' compound_expr '}' { $$ = new Ides::BlockExpr($1, $3); }
             | postfix_expr '.' ident { $$ = new Ides::DotExpr($1, $3); }
             | postfix_expr KW_VARARGS { $$ = new Ides::VarArgsExpr($1); }
             | primary_expr
;

prefix_expr : operator prefix_expr { $$ = new Ides::PrefixExpr($2, $1); }
            | postfix_expr
;

if_expr : KW_IF '(' expr ')' expr { $$ = new Ides::IfExpr($3, $5); }
;

else_expr : if_expr
          | else_expr KW_ELSE expr { $$ = Ides::InfixExpr::Create(new Ides::IdentifierExpr("else"), $1, $3); }
;

infix_expr : infix_expr operator prefix_expr { $$ = Ides::InfixExpr::Create($2, $1, $3); }
           | prefix_expr
;

expr : infix_expr
;


tuple_items : { $$ = new Ides::TupleExpr(); }
            | expr { $$ = new Ides::TupleExpr(); $$->Add($1); }
            | tuple_items ',' expr { $$ = $1; $$->Add($3); }
;

arg_items : { $$ = new Ides::TupleExpr(); }
          | TIDENTIFIER ':' expr { $$ = new Ides::TupleExpr(); $$->Add(new Ides::ArgDecl(Ides::V_DEFAULT, $1, $3)); }
          | arg_items ',' TIDENTIFIER ':' expr { $$ = $1; $$->Add(new Ides::ArgDecl(Ides::V_DEFAULT, $3, $5)); }
;

val_decl : vis KW_VAL name ':' expr { $$ = new Ides::ValDecl($1, $3, new Ides::ValueDecl<Ides::VAL>($5, NULL)); }
         | vis KW_VAR name ':' expr { $$ = new Ides::VarDecl($1, $3, new Ides::ValueDecl<Ides::VAR>($5, NULL)); }
         | vis KW_VAL name '=' expr { $$ = new Ides::ValDecl($1, $3, new Ides::ValueDecl<Ides::VAL>(NULL, $5)); }
         | vis KW_VAR name '=' expr { $$ = new Ides::VarDecl($1, $3, new Ides::ValueDecl<Ides::VAR>(NULL, $5)); }
;

vis : vis_extern;

vis_extern : vis_const
           | vis_const KW_EXTERN
;

vis_const : vis_abstract
          | vis_abstract KW_CONST { $$ = (Ides::Visibility)($1 | Ides::V_CONST); }
;

vis_abstract : vis_type
             | vis_type KW_ABSTRACT { $$ = (Ides::Visibility)($1 | Ides::V_ABSTRACT); }
;

vis_type : { $$ = Ides::V_DEFAULT; }
         | KW_PUBLIC { $$ = Ides::V_PUBLIC; }
         | KW_PRIVATE { $$ = Ides::V_PRIVATE; }
         | KW_PROTECTED { $$ = Ides::V_PROTECTED; }
         | KW_INTERNAL { $$ = Ides::V_INTERNAL; }
;

trait_decl : vis KW_TRAIT name data_decl { $$ = new Ides::TraitDecl($1, $3, $4); }
;

class_decl : vis KW_CLASS name data_decl { $$ = new Ides::ClassDecl($1, $3, $4); }
;

struct_decl : vis KW_STRUCT name data_decl { $$ = new Ides::StructDecl($1, $3, $4); }
;

fn_decl : vis KW_DEF name '(' arg_items ')' ':' expr KW_DEFINEDAS expr { $$ = new Ides::FnDecl($1, $3, new Ides::FunctionDecl($5, $8, $10)); }
        | vis KW_DEF name '(' arg_items ')' KW_DEFINEDAS expr { $$ = new Ides::FnDecl($1, $3, new Ides::FunctionDecl($5, NULL, $8)); }
        | vis KW_DEF name '(' arg_items ')' expr { $$ = new Ides::FnDecl($1, $3, new Ides::FunctionDecl($5, new Ides::IdentifierExpr("void"), $7)); }
        | vis KW_DEF name '(' arg_items ')' ':' expr ';' { $$ = new Ides::FnDecl($1, $3, new Ides::FunctionDecl($5, $8, NULL)); }
        | vis KW_DEF name '(' arg_items ')' ';' { $$ = new Ides::FnDecl($1, $3, new Ides::FunctionDecl($5, new Ides::IdentifierExpr("void"), NULL)); }
;

data_decl : ':' tuple_items KW_DEFINEDAS '{' compound_expr '}' { $$ = new Ides::DataStructureDecl(new Ides::TupleExpr(), $2, $5); }
          | KW_DEFINEDAS '{' compound_expr '}' { $$ = new Ides::DataStructureDecl(new Ides::TupleExpr(), new Ides::TupleExpr(), $3); }
          | '(' arg_items ')' ':' tuple_items KW_DEFINEDAS '{' compound_expr '}' { $$ = new Ides::DataStructureDecl($2, $5, $8); }
          | '(' arg_items ')' KW_DEFINEDAS '{' compound_expr '}' { $$ = new Ides::DataStructureDecl($2, new Ides::TupleExpr(), $6); }
;

pf_items : KW_CASE expr KW_DEFINEDAS expr { $$ = new Ides::PartialFunction(); $$->Add(new Ides::CasePair($2, $4)); }
         | pf_items KW_CASE expr KW_DEFINEDAS expr { $$ = $1; $$->Add(new Ides::CasePair($3, $5)); }
;

%%
