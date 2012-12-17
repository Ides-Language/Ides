%{
    #include <iostream>
    #include <ides/Parsing/ParserCommon.h>
    #include <stdexcept>
    #include <sstream>
    
    void yyerror(YYLTYPE* locp, Ides::Parsing::Parser* context, Ides::AST::ASTCompilationUnit** output, const char* err)
	{
        throw Ides::Diagnostics::CompileError(err, *locp);
	}
    
    #define SET_EXPRLOC(x, loc) (x)->exprloc = (loc)
    #define UNARY_EXPR(x, arg) (Ides::AST::ASTExpression*)new Ides::AST::AST##x##Expression(arg)
    #define BINARY_EXPR(x, lhs, rhs) (Ides::AST::ASTExpression*)new Ides::AST::AST##x##Expression(lhs, rhs)
%}

%pure-parser
%defines
%error-verbose
%locations
%debug


%parse-param { Ides::Parsing::Parser* context }
%parse-param { Ides::AST::ASTCompilationUnit** output }
%lex-param { void* scanner  }

%union {
    /***** AST Elements *****/
    Ides::AST::AST* ast_base;
    Ides::AST::ASTIdentifier* ast_ident;
    Ides::AST::ASTList* ast_list;
    Ides::AST::ASTCompilationUnit* ast_prog;
    
    Ides::AST::ASTType* ast_type;
    Ides::AST::ASTPtrType* ast_ptr;
    Ides::AST::ASTTypeName* ast_basetype;
    
    Ides::AST::ASTExpression* ast_expr;
    Ides::AST::ASTStatement* ast_stmt;
    
    Ides::AST::ASTCompoundStatement* ast_block;
    
    Ides::AST::ASTDeclaration* ast_decl;
    Ides::AST::ASTFunction* ast_fn;
    
    Ides::AST::ASTConstantExpression* ast_const;
    Ides::AST::ASTConstantIntExpression* ast_int;
    Ides::AST::ASTConstantStringExpression* ast_istr;
    Ides::AST::ASTConstantCStringExpression* ast_cstr;
    Ides::AST::ASTConstantWCStringExpression* ast_wstr;
    Ides::AST::ASTConstantLCStringExpression* ast_lstr;
    
    /***** Type Elements *****/
    Ides::Types::Type* type_base;
}

%{
    
    int yylex(YYSTYPE* lvalp, YYLTYPE* llocp, void* scan);
    
    #define scanner context->GetScanner()
    
%}

// Builtin types
%token KW_INT8 KW_INT16 KW_INT32 KW_INT64

// Keywords
%token KW_DEF KW_VAR KW_VAL KW_THROW KW_NEW KW_IF KW_EXTERN KW_NULL KW_RETURN
// Operators
%token OP_INC OP_DEC OP_EQ OP_COALESCE OP_CAST

%token <ast_int> TINTEGER
%token <ast_ident> TIDENTIFIER
%token <ast_base> TCHAR
%token <ast_istr> TSTRING
%token <ast_base> TDOUBLE

%type <ast_prog> program program_decl program_decl_list

%type <ast_const> literal
%type <ast_fn> fn_decl fn_def extern_def
%type <ast_list> dictionary_expression array_expression
%type <ast_list> arg_decl_list arg_val_list dictionary_val_list

%type <ast_decl> var_decl val_decl arg_decl
%type <ast_type> var_type

%type <ast_expr> expression infix_expression binary_expression unary_expression postfix_expression primary_expression
%type <ast_stmt> stmt
%type <ast_block> stmt_list

%type <ast_prog> root


/* Operator precedence for mathematical operators */
%left '='
%left OP_COALESCE OP_CAST
%left '+' '-'
%left '*' '/' '%'

%start root

%%

root : program { *output = $1; }
;

program : program_decl_list
;

program_decl : fn_def
            // | class_def
            // | struct_def
            // | import_stmt
             | var_decl ';'
             | val_decl ';'
             | extern_def
;

program_decl_list : program_decl { $$ = new Ides::AST::ASTCompilationUnit(); $$->push_back($1); }
                  | program_decl_list program_decl { $$ = $1; $$->push_back($2); }
;

primary_expression : TIDENTIFIER { SET_EXPRLOC($$, @$); }
                   | TINTEGER { SET_EXPRLOC($$, @$); }
                   | TSTRING { SET_EXPRLOC($$, @$); }
                   | TDOUBLE { SET_EXPRLOC($$, @$); }
                   | TCHAR { SET_EXPRLOC($$, @$); }
                   | KW_NULL { $$ = new Ides::AST::ASTNullExpr(); SET_EXPRLOC($$, @$); }
                   //| array_expression
                   //| dictionary_expression
                   | '(' expression ')' { $$ = $2; }
;

array_expression : '[' arg_val_list ']' { $$ = $2; }
;

dictionary_expression : '{' dictionary_val_list '}' { $$ = $2; }
;

dictionary_val_list : expression ':' expression { $$ = new Ides::AST::ASTDictExpression(); $$->push_back(BINARY_EXPR(DictPair, $1, $3)); }
                    | dictionary_val_list ',' expression ':' expression { $$ = $1; $$->push_back(BINARY_EXPR(DictPair, $3, $5)); }
;

postfix_expression : primary_expression
                   | postfix_expression OP_INC { $$ = UNARY_EXPR(PostInc, $1); SET_EXPRLOC($$, @$); }
                   | postfix_expression OP_DEC { $$ = UNARY_EXPR(PostDec, $1); SET_EXPRLOC($$, @$); }
                   | postfix_expression '(' ')' { $$ = new Ides::AST::ASTFunctionCall($1, NULL); SET_EXPRLOC($$, @$); }
                   | postfix_expression '(' arg_val_list ')' { $$ = new Ides::AST::ASTFunctionCall($1, $3); SET_EXPRLOC($$, @$); }
                   | postfix_expression '[' arg_val_list ']' { $$ = new Ides::AST::ASTBracketCall($1, $3); SET_EXPRLOC($$, @$); }
                   | postfix_expression '.' TIDENTIFIER
;

unary_expression : postfix_expression
                 | '*' unary_expression { $$ = UNARY_EXPR(Deref, $2); SET_EXPRLOC($$, @$); }
                 | '!' unary_expression { $$ = UNARY_EXPR(Not, $2); SET_EXPRLOC($$, @$); }
                 | '~' unary_expression { $$ = UNARY_EXPR(Bnot, $2); SET_EXPRLOC($$, @$); }
                 | '-' unary_expression { $$ = UNARY_EXPR(Neg, $2); SET_EXPRLOC($$, @$); }
                 | KW_NEW unary_expression { $$ = UNARY_EXPR(New, $2); SET_EXPRLOC($$, @$); }
                 | KW_THROW unary_expression { $$ = UNARY_EXPR(Throw, $2); SET_EXPRLOC($$, @$); }
                 | KW_RETURN unary_expression { $$ = UNARY_EXPR(Return, $2); SET_EXPRLOC($$, @$); }
                 | OP_INC unary_expression { $$ = UNARY_EXPR(PreInc, $2); SET_EXPRLOC($$, @$); }
                 | OP_DEC unary_expression { $$ = UNARY_EXPR(PreDec, $2); SET_EXPRLOC($$, @$); }
;

binary_expression : unary_expression
                 | binary_expression '=' binary_expression { $$ = BINARY_EXPR(Eq, $1, $3); SET_EXPRLOC($$, @$); }
                 | binary_expression '+' binary_expression { $$ = BINARY_EXPR(Add, $1, $3); SET_EXPRLOC($$, @$); }
                 | binary_expression '-' binary_expression { $$ = BINARY_EXPR(Sub, $1, $3); SET_EXPRLOC($$, @$); }
                 | binary_expression '*' binary_expression { $$ = BINARY_EXPR(Mul, $1, $3); SET_EXPRLOC($$, @$); }
                 | binary_expression '/' binary_expression { $$ = BINARY_EXPR(Div, $1, $3); SET_EXPRLOC($$, @$); }
                 | binary_expression '%' binary_expression { $$ = BINARY_EXPR(Mod, $1, $3); SET_EXPRLOC($$, @$); }

                 | binary_expression '&' binary_expression { $$ = BINARY_EXPR(Band, $1, $3); SET_EXPRLOC($$, @$); }
                 | binary_expression '|' binary_expression { $$ = BINARY_EXPR(Bor, $1, $3); SET_EXPRLOC($$, @$); }
                 | binary_expression '^' binary_expression { $$ = BINARY_EXPR(Bxor, $1, $3); SET_EXPRLOC($$, @$); }

                 | binary_expression OP_COALESCE binary_expression { $$ = BINARY_EXPR(Coalesce, $1, $3); SET_EXPRLOC($$, @$); }
                 | binary_expression OP_CAST binary_expression { $$ = BINARY_EXPR(Cast, $1, $3); SET_EXPRLOC($$, @$); }
;

infix_expression : binary_expression
                 | infix_expression TIDENTIFIER infix_expression { $$ = new Ides::AST::ASTInfixExpression($2, $1, $3); SET_EXPRLOC($$, @$); }
;

expression : infix_expression
;

var_type : TIDENTIFIER { $$ = new Ides::AST::ASTTypeName($1); SET_EXPRLOC($$, @$); }
         | KW_INT8 { $$ = new Ides::AST::ASTInteger8Type(); SET_EXPRLOC($$, @$); }
         | KW_INT16 { $$ = new Ides::AST::ASTInteger16Type(); SET_EXPRLOC($$, @$); }
         | KW_INT32 { $$ = new Ides::AST::ASTInteger32Type(); SET_EXPRLOC($$, @$); }
         | KW_INT64 { $$ = new Ides::AST::ASTInteger64Type(); SET_EXPRLOC($$, @$); }
         | var_type '*' { $$ = new Ides::AST::ASTPtrType($1); SET_EXPRLOC($$, @$); }
;

var_decl : KW_VAR TIDENTIFIER '=' expression { $$ = new Ides::AST::ASTDeclaration(Ides::AST::ASTDeclaration::DECL_VAR, $2, $4); SET_EXPRLOC($$, @$); }
         | KW_VAR TIDENTIFIER ':' var_type { $$ = new Ides::AST::ASTDeclaration(Ides::AST::ASTDeclaration::DECL_VAR, $2, $4); SET_EXPRLOC($$, @$); }
         | KW_VAR TIDENTIFIER ':' var_type '=' expression { $$ = new Ides::AST::ASTDeclaration(Ides::AST::ASTDeclaration::DECL_VAR, $2, $4, $6); SET_EXPRLOC($$, @$); }
;

val_decl : KW_VAL TIDENTIFIER '=' expression { $$ = new Ides::AST::ASTDeclaration(Ides::AST::ASTDeclaration::DECL_VAL, $2, $4); SET_EXPRLOC($$, @$); }
         | KW_VAL TIDENTIFIER ':' var_type { $$ = new Ides::AST::ASTDeclaration(Ides::AST::ASTDeclaration::DECL_VAL, $2, $4); SET_EXPRLOC($$, @$); }
         | KW_VAL TIDENTIFIER ':' var_type '=' expression { $$ = new Ides::AST::ASTDeclaration(Ides::AST::ASTDeclaration::DECL_VAL, $2, $4, $6); SET_EXPRLOC($$, @$); }
;


arg_decl : var_decl
         | val_decl
;

arg_decl_list : arg_decl { $$ = new Ides::AST::ASTList(); $$->push_back($1); SET_EXPRLOC($$, @$); }
              | arg_decl_list ',' arg_decl { $$ = $1; $$->push_back($3); SET_EXPRLOC($$, @$); }
;

arg_val_list : expression { $$ = new Ides::AST::ASTList(); $$->push_back($1); SET_EXPRLOC($$, @$); }
             | arg_val_list ',' expression { $$ = $1; $$->push_back($3); SET_EXPRLOC($$, @$); }
;


fn_decl : KW_DEF TIDENTIFIER '(' arg_decl_list ')' { $$ = new Ides::AST::ASTFunction($2, $4, NULL); SET_EXPRLOC($$, @$); }
        | KW_DEF TIDENTIFIER '(' arg_decl_list ')' ':' var_type { $$ = new Ides::AST::ASTFunction($2, $4, $7); SET_EXPRLOC($$, @$); }
        | KW_DEF TIDENTIFIER '(' ')' { $$ = new Ides::AST::ASTFunction($2, NULL, NULL); SET_EXPRLOC($$, @$); }
        | KW_DEF TIDENTIFIER '(' ')' ':' var_type { $$ = new Ides::AST::ASTFunction($2, NULL, $6); SET_EXPRLOC($$, @$); }
;

fn_def  : fn_decl '{' stmt_list '}' { $$ = $1; $$->body = $3; }
        | fn_decl '{' '}'
        | fn_decl '=' expression ';' { $$ = $1; $$->val = $3; }
;

extern_def : KW_EXTERN fn_decl ';' { $$ = $2; }
;

if_stmt : KW_IF '(' expression ')' stmt
;

stmt : var_decl ';'
     | val_decl ';'
     | expression ';'
;

stmt_list : stmt { $$ = new Ides::AST::ASTCompoundStatement(); }
          | stmt_list stmt { $$ = $1; $$->push_back($2); }
          | stmt_list ';' { $$ = $1; }
;

literal : TINTEGER
/*        | TSTRING
        | TDOUBLE
        | TCHAR */
;

%%
