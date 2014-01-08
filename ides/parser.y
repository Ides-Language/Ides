%{
    #include <iostream>
    #include <ides/Source/SourceLocation.h>
    #include <ides/Parsing/Parser.h>
    #include <ides/lexer.hpp>
    #include <stdexcept>
    #include <sstream>

#define YYLTYPE Ides::SourceRange
#define YYLLOC_DEFAULT(Current, Rhs, N) {\
    if (N) { \
        YYRHSLOC(Rhs, 1).begin.file = YYRHSLOC(Rhs, N).begin.file = context->GetSourceFile(); \
        (Current) = YYRHSLOC(Rhs, 1).Union(YYRHSLOC(Rhs, N)); \
    }  else { \
        (Current).begin = YYRHSLOC(Rhs, 0).begin; (Current).length = 0; \
    } \
}

#undef YYLTYPE_IS_TRIVIAL

    void yyerror(YYLTYPE* locp, Ides::Parser* context, Ides::Ast** output, const char* err)
	{ SETTRACE("yyerror")
        MSG(E_PARSE) % err % context->GetSourceFile()->GetLineForOffset(locp->begin.offset)->number;
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
    Ides::AstBase* ast;
    Ides::Expr* expr;
    Ides::Decl* decl;
    Ides::DataStructureDecl* decl_type;

    Ides::ExprList* tuple_expr;
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
    yylval.ast = NULL;
}

// Builtin types
%token KW_VOID KW_UNIT KW_BOOL KW_INT8 KW_UINT8 KW_INT16 KW_UINT16 KW_INT32 KW_UINT32 KW_INT64 KW_UINT64 KW_FLOAT32 KW_FLOAT64

// Specifiers
%token KW_PUBLIC KW_PROTECTED KW_INTERNAL KW_PRIVATE KW_EXTERN KW_CONST KW_ABSTRACT

// Keywords
%token KW_DEF KW_FN KW_STRUCT KW_CLASS KW_TRAIT KW_MOD KW_VAR KW_VAL KW_TYPE KW_NULL KW_NAMESPACE KW_CASE
%token KW_IF KW_ELSE
%token KW_DEFINEDAS
%token KW_VARARGS
// Keyword operators
%token KW_THROW KW_NEW KW_RETURN KW_MATCH KW_AS

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
%type <expr> constant primary_expr prefix_expr postfix_expr infix_expr if_expr expr stmt
%type <decl> val_decl trait_decl class_decl struct_decl mod_decl fn_decl
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
         | '=' { $$ = new Ides::IdentifierExpr("="); $$->source = @$; }
         | KW_MATCH { $$ = new Ides::IdentifierExpr("match"); $$->source = @$; }
         | KW_AS { $$ = new Ides::IdentifierExpr("as"); $$->source = @$; }
         | KW_IF { $$ = new Ides::IdentifierExpr("if"); }
         | KW_ELSE { $$ = new Ides::IdentifierExpr("else"); }
;

ident : TOPERATOR
      | TIDENTIFIER
;

stmt_end :
         | ';'
;

stmt : expr stmt_end
     | fn_decl stmt_end
     | trait_decl stmt_end
     | class_decl stmt_end
     | struct_decl stmt_end
     | mod_decl stmt_end
;

name : ident { $$ = new Ides::Name($1); $$->source = @$; }
     | ident '[' arg_items ']' { $$ = new Ides::Name($1, $3); $$->source = @$; }
;

compound_expr : { $$ = new Ides::ExprList(); }
              | compound_expr stmt { $$ = $1; $$->Add($2); $$->source = @$; }
;

primary_expr : constant
             | TIDENTIFIER
             | TPLACEHOLDER
             | val_decl
             | '(' tuple_items ')' { $$ = $2; }
             | '{' compound_expr '}' { $$ = $2; }
             | '{' pf_items '}' { $$ = $2; }
;

postfix_expr : postfix_expr '(' tuple_items ')' { $$ = new Ides::CallExpr($1, $3); $$->source = @$; }
             | postfix_expr '[' tuple_items ']' { $$ = new Ides::IndexExpr($1, $3); $$->source = @$; }
             | postfix_expr '{' compound_expr '}' { $$ = new Ides::UnaryExpr($1, $3, false); $$->source = @$; }
             | postfix_expr '.' ident { $$ = Ides::BinaryExpr::Create(new Ides::IdentifierExpr("."), $1, $3); $$->source = @$; }
             | postfix_expr KW_VARARGS { $$ = new Ides::UnaryExpr(new Ides::IdentifierExpr("..."), $1, false); $$->source = @$; }
             | primary_expr
;

prefix_expr : operator prefix_expr { $$ = new Ides::UnaryExpr($1, $2); $$->source = @$; }
            | KW_IF '(' expr ')' expr { $$ = new Ides::BinaryExpr(new Ides::IdentifierExpr("if"), $5, $3); $$->source = @$; }
            | postfix_expr
;

if_expr : KW_IF '(' expr ')' expr { $$ = new Ides::BinaryExpr(new Ides::IdentifierExpr("if"), $5, $3); $$->source = @$; }
;

infix_expr : infix_expr operator prefix_expr { $$ = Ides::BinaryExpr::Create($2, $1, $3); $$->source = @$; }
           | prefix_expr
;

expr : infix_expr
;


tuple_items : { $$ = new Ides::ExprList(); $$->source = @$; }
            | expr { $$ = new Ides::ExprList(); $$->Add($1); $$->source = @$; }
            | tuple_items ',' expr { $$ = $1; $$->Add($3); $$->source = @$; }
;

arg_items : { $$ = new Ides::ExprList(); $$->source = @$; }
          | TIDENTIFIER ':' expr { $$ = new Ides::ExprList(); $$->Add(new Ides::ArgDecl(Ides::V_DEFAULT, $1, $3)); $$->source = @$; }
          | arg_items ',' TIDENTIFIER ':' expr { $$ = $1; $$->Add(new Ides::ArgDecl(Ides::V_DEFAULT, $3, $5)); $$->source = @$; }
;

val_decl : vis KW_VAR name ':' expr KW_DEFINEDAS expr { $$ = new Ides::VarDecl($1, $3, new Ides::ValueDecl($5, $7)); $$->source = @$; }
         | vis KW_VAR name ':' expr { $$ = new Ides::VarDecl($1, $3, new Ides::ValueDecl($5, NULL)); $$->source = @$; }
         | vis KW_VAR name '=' expr { $$ = new Ides::VarDecl($1, $3, new Ides::ValueDecl(NULL, $5)); $$->source = @$; }
         | vis KW_VAL name ':' expr KW_DEFINEDAS expr { $$ = new Ides::ValDecl($1, $3, new Ides::ValueDecl($5, $7)); $$->source = @$; }
         | vis KW_VAL name ':' expr { $$ = new Ides::ValDecl($1, $3, new Ides::ValueDecl($5, NULL)); $$->source = @$; }
         | vis KW_VAL name '=' expr { $$ = new Ides::ValDecl($1, $3, new Ides::ValueDecl(NULL, $5)); $$->source = @$; }
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

trait_decl : vis KW_TRAIT name data_decl { $$ = new Ides::TraitDecl($1, $3, $4); $$->source = @$; }
;

class_decl : vis KW_CLASS name data_decl { $$ = new Ides::ClassDecl($1, $3, $4); $$->source = @$; }
;

struct_decl : vis KW_STRUCT name data_decl { $$ = new Ides::StructDecl($1, $3, $4); $$->source = @$; }
;

mod_decl : vis KW_MOD name '{' compound_expr '}' { $$ = new Ides::ModuleDecl($1, $3, $5); $$->source = @$; }
;

fn_decl : vis KW_DEF name '(' arg_items ')' ':' expr KW_DEFINEDAS expr { $$ = new Ides::FnDecl($1, $3, new Ides::FnDataDecl($5, $8, $10)); $$->source = @$; }
        | vis KW_DEF name '(' arg_items ')' KW_DEFINEDAS expr { $$ = new Ides::FnDecl($1, $3, new Ides::FnDataDecl($5, NULL, $8)); $$->source = @$; }
        | vis KW_DEF name '(' arg_items ')' expr { $$ = new Ides::FnDecl($1, $3, new Ides::FnDataDecl($5, new Ides::IdentifierExpr("void"), $7)); $$->source = @$; }
        | vis KW_DEF name '(' arg_items ')' ':' expr ';' { $$ = new Ides::FnDecl($1, $3, new Ides::FnDataDecl($5, $8, NULL)); $$->source = @$; }
        | vis KW_DEF name '(' arg_items ')' ';' { $$ = new Ides::FnDecl($1, $3, new Ides::FnDataDecl($5, new Ides::IdentifierExpr("void"), NULL)); $$->source = @$; }
;

data_decl : ':' tuple_items KW_DEFINEDAS '{' compound_expr '}' { $$ = new Ides::DataStructureDecl(new Ides::ExprList(), $2, $5); $$->source = @$; }
          | KW_DEFINEDAS '{' compound_expr '}' { $$ = new Ides::DataStructureDecl(new Ides::ExprList(), new Ides::ExprList(), $3); $$->source = @$; }
          | '(' arg_items ')' ':' tuple_items KW_DEFINEDAS '{' compound_expr '}' { $$ = new Ides::DataStructureDecl($2, $5, $8); $$->source = @$; }
          | '(' arg_items ')' KW_DEFINEDAS '{' compound_expr '}' { $$ = new Ides::DataStructureDecl($2, new Ides::ExprList(), $6); $$->source = @$; }
;

pf_items : KW_CASE expr KW_DEFINEDAS expr { $$ = new Ides::PartialFunction(); $$->Add($2, $4); $$->source = @$; }
         | pf_items KW_CASE expr KW_DEFINEDAS expr { $$ = $1; $$->Add($3, $5); $$->source = @$; }
         | pf_items KW_CASE KW_ELSE KW_DEFINEDAS expr { $$ = $1; $$->Add(NULL, $5); $$->source = @$; }
;

%%
