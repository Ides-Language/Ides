%{
    #include <iostream>
    #include <ides/Source/SourceLocation.h>
    #include <ides/Parsing/Parser.h>
    #include <ides/lexer.hpp>
    #include <ides/Lang.h>
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
    Ides::QualExpr* qual;

    Ides::ConstantInt* ast_int;
    Ides::ConstantString* ast_str;
    Ides::ConstantChar* ast_char;
    Ides::ConstantDec* ast_dec;
    Ides::ConstantBool* ast_bool;
    Ides::PlaceholderExpr* ast_placeholder;
    Ides::IdentifierExpr* ast_ident;

    Ides::ValKind val_kind;
    Ides::RecordKind data_kind;

    uint8_t placeholder;
}

%initial-action {
    yylval.ast = NULL;
}

// Builtin types
%token KW_VOID KW_UNIT KW_BOOL KW_INT8 KW_UINT8 KW_INT16 KW_UINT16 KW_INT32 KW_UINT32 KW_INT64 KW_UINT64 KW_FLOAT32 KW_FLOAT64

// Specifiers
%token KW_PUBLIC KW_PROTECTED KW_INTERNAL KW_PRIVATE KW_EXTERN KW_CONST KW_ABSTRACT KW_UNSAFE KW_INTRINSIC KW_IMPLICIT KW_LOCKED

// Keywords
%token KW_STRUCT KW_CLASS KW_TRAIT KW_IMPL
%token KW_DEF KW_FN KW_MOD KW_VAR KW_VAL KW_TYPE KW_NULL KW_NAMESPACE KW_CASE
%token KW_IF KW_ELSE
%token KW_DEFINEDAS
%token KW_VARARGS KW_RANGE
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
%type <expr> constant primary_expr prefix_expr postfix_expr infix_expr if_expr arg_item expr stmt
%type <decl> val_decl trait_decl class_decl struct_decl impl_decl mod_decl fn_decl
%type <decl_type> data_decl
%type <tuple_expr> compound_expr tuple_items arg_items name_items
%type <pf_expr> pf_items

%type <qual> qual
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
         | KW_RANGE { $$ = new Ides::IdentifierExpr(".."); }
;

ident : TOPERATOR
      | TIDENTIFIER
;

stmt_end :
         | ';'
;

stmt : expr ';'
     | fn_decl stmt_end
     | trait_decl stmt_end
     | class_decl stmt_end
     | struct_decl stmt_end
     | mod_decl stmt_end
;

name : ident { $$ = new Ides::Name($1); $$->source = @$; }
     | ident '[' tuple_items ']' { $$ = new Ides::Name($1, $3); $$->source = @$; }
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
             | '(' arg_items ')' KW_DEFINEDAS expr { $$ = new Ides::FnDataDecl($2, NULL, $5); $$->source = @$; }
             | '(' arg_items ')' ':' name KW_DEFINEDAS expr { $$ = new Ides::FnDataDecl($2, $5, $7); $$->source = @$; }
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



name_items : { $$ = new Ides::ExprList(); $$->source = @$; }
           | name { $$ = new Ides::ExprList(); $$->Add($1); $$->source = @$; }
           | name_items ',' name { $$ = $1; $$->Add($3); $$->source = @$; }
;

tuple_items : { $$ = new Ides::ExprList(); $$->source = @$; }
            | expr { $$ = new Ides::ExprList(); $$->Add($1); $$->source = @$; }
            | tuple_items ',' expr { $$ = $1; $$->Add($3); $$->source = @$; }
;

arg_item : qual TIDENTIFIER ':' name { $$ = new Ides::ArgDecl($1, $2, $4); $$->source = @$; }
         //| TIDENTIFIER { $$ = new Ides::ArgDecl(new Ides::QualExpr(), $1, NULL); $$->source = @$; }
;

arg_items : { $$ = new Ides::ExprList(); $$->source = @$; }
          | arg_item { $$ = new Ides::ExprList(); $$->Add($1); $$->source = @$; }
          | arg_items ',' arg_item { $$ = $1; $$->Add($3); $$->source = @$; }
;

val_decl : qual KW_VAR name ':' name '=' expr { $$ = new Ides::VarDecl($1, $3, new Ides::ValueDecl($5, $7)); $$->source = @$; }
         | qual KW_VAR name ':' name { $$ = new Ides::VarDecl($1, $3, new Ides::ValueDecl($5, NULL)); $$->source = @$; }
         | qual KW_VAR name '=' expr { $$ = new Ides::VarDecl($1, $3, new Ides::ValueDecl(NULL, $5)); $$->source = @$; }
         | qual KW_VAL name ':' name '=' expr { $$ = new Ides::ValDecl($1, $3, new Ides::ValueDecl($5, $7)); $$->source = @$; }
         | qual KW_VAL name ':' name { $$ = new Ides::ValDecl($1, $3, new Ides::ValueDecl($5, NULL)); $$->source = @$; }
         | qual KW_VAL name '=' expr { $$ = new Ides::ValDecl($1, $3, new Ides::ValueDecl(NULL, $5)); $$->source = @$; }
;

qual : { $$ = new Ides::QualExpr(); }
     | qual KW_PUBLIC { $$ = $1; $$->set(Ides::QUAL_PUBLIC); }
     | qual KW_PRIVATE { $$ = $1; $$->set(Ides::QUAL_PRIVATE); }
     | qual KW_PROTECTED { $$ = $1; $$->set(Ides::QUAL_PROTECTED); }
     | qual KW_INTERNAL { $$ = $1; $$->set(Ides::QUAL_INTERNAL); }
     | qual KW_ABSTRACT { $$ = $1; $$->set(Ides::QUAL_ABSTRACT); }
     | qual KW_CONST { $$ = $1; $$->set(Ides::QUAL_CONST); }
     | qual KW_EXTERN { $$ = $1; $$->set(Ides::QUAL_EXTERN); }
     | qual KW_UNSAFE { $$ = $1; $$->set(Ides::QUAL_UNSAFE); }
     | qual KW_INTRINSIC { $$ = $1; $$->set(Ides::QUAL_INTRINSIC); }
     | qual KW_IMPLICIT { $$ = $1; $$->set(Ides::QUAL_IMPLICIT); }
     | qual KW_LOCKED { $$ = $1; $$->set(Ides::QUAL_LOCKED); }
;

trait_decl : qual KW_TRAIT name data_decl { $$ = new Ides::RecordDecl($1, Ides::TRAIT, $3, $4); $$->source = @$; }
;

class_decl : qual KW_CLASS name data_decl { $$ = new Ides::RecordDecl($1, Ides::CLASS, $3, $4); $$->source = @$; }
;

struct_decl : qual KW_STRUCT name data_decl { $$ = new Ides::RecordDecl($1, Ides::STRUCT, $3, $4); $$->source = @$; }
;

impl_decl : qual KW_IMPL name data_decl { $$ = new Ides::RecordDecl($1, Ides::IMPL, $3, $4); $$->source = @$; }
;

mod_decl : qual KW_MOD name '{' compound_expr '}' { $$ = new Ides::ModuleDecl($1, $3, $5); $$->source = @$; }
;

fn_decl : qual KW_DEF name '(' arg_items ')' ':' name '=' expr { $$ = new Ides::FnDecl($1, $3, new Ides::FnDataDecl($5, $8, $10)); $$->source = @$; }
        | qual KW_DEF name '(' arg_items ')' '=' expr { $$ = new Ides::FnDecl($1, $3, new Ides::FnDataDecl($5, NULL, $8)); $$->source = @$; }
        | qual KW_DEF name '(' arg_items ')' ':' name ';' { $$ = new Ides::FnDecl($1, $3, new Ides::FnDataDecl($5, $8, NULL)); $$->source = @$; }
        | qual KW_DEF name '(' arg_items ')' ';' { $$ = new Ides::FnDecl($1, $3, new Ides::FnDataDecl($5, NULL, NULL)); $$->source = @$; }
;

data_decl : ':' name_items '{' compound_expr '}' { $$ = new Ides::DataStructureDecl(new Ides::ExprList(), $2, $4); $$->source = @$; }
          | '{' compound_expr '}' { $$ = new Ides::DataStructureDecl(new Ides::ExprList(), new Ides::ExprList(), $2); $$->source = @$; }
          | '(' arg_items ')' ':' name_items '{' compound_expr '}' { $$ = new Ides::DataStructureDecl($2, $5, $7); $$->source = @$; }
          | '(' arg_items ')' '{' compound_expr '}' { $$ = new Ides::DataStructureDecl($2, new Ides::ExprList(), $5); $$->source = @$; }
;

pf_items : KW_CASE expr KW_DEFINEDAS expr { $$ = new Ides::PartialFunction(); $$->Add($2, $4); $$->source = @$; }
         | pf_items KW_CASE expr KW_DEFINEDAS expr { $$ = $1; $$->Add($3, $5); $$->source = @$; }
         | pf_items KW_CASE KW_ELSE KW_DEFINEDAS expr { $$ = $1; $$->Add(NULL, $5); $$->source = @$; }
;

%%
