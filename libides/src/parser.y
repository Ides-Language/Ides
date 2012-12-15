%{
    #include <iostream>
    #include <ides/Parsing/ParserCommon.h>
    
    void yyerror(YYLTYPE* locp, Ides::Parsing::Parser* context, const char* err)
	{
		std::cout << err << std::endl;
	}
    
    #define UNARY_EXPR(x, arg) (Ides::AST::ASTExpression*)new Ides::AST::AST##x##Expression(arg)
    #define BINARY_EXPR(x, lhs, rhs) (Ides::AST::ASTExpression*)new Ides::AST::AST##x##Expression(lhs, rhs)
%}

%pure-parser
%defines
%error-verbose
%locations
%debug

%parse-param { Ides::Parsing::Parser* context }
%lex-param { void* scanner  }

%union {
    Ides::AST::AST* ast_base;
    Ides::AST::ASTIdentifier* ast_ident;
    Ides::AST::ASTList* ast_list;
    Ides::AST::ASTCompilationUnit* ast_prog;
    
    Ides::AST::ASTDeclaration* ast_decl;
    Ides::AST::ASTFunction* ast_fn;
    
    Ides::AST::ASTExpression* ast_expr;
    
    Ides::AST::ASTConstantExpression* ast_const;
    Ides::AST::ASTConstantIntExpression* ast_int;
    Ides::AST::ASTConstantStringExpression* ast_istr;
    Ides::AST::ASTConstantCStringExpression* ast_cstr;
    Ides::AST::ASTConstantWCStringExpression* ast_wstr;
    Ides::AST::ASTConstantLCStringExpression* ast_lstr;
}

%{
    
    int yylex(YYSTYPE* lvalp, YYLTYPE* llocp, void* scan);
    
    #define scanner context->GetScanner()
    
%}

// Keywords
%token KW_DEF KW_VAR KW_VAL KW_OR
// Operators
%token OP_INC OP_DEC

%token <ast_int> TINTEGER
%token <ast_ident> TIDENTIFIER
%token <ast_base> TCHAR
%token <ast_istr> TSTRING
%token <ast_base> TDOUBLE

%type <ast_prog> program
%type <ast_const> literal
%type <ast_fn> fn_decl fn_def
%type <ast_list> fn_list stmt_list
%type <ast_list> arg_decl_list arg_val_list

%type <ast_decl> var_decl val_decl arg_decl

%type <ast_expr> expression infix_expression unary_expression postfix_expression primary_expression


/* Operator precedence for mathematical operators */
%left '='
%left KW_OR
%left '+' '-'
%left '*' '/' '%'
%left '.'

%start program

%%

program : fn_list { $$ = new Ides::AST::ASTCompilationUnit($1); std::cout << $$->GetDOT() << std::endl; };


fn_list : fn_def { $$ = new Ides::AST::ASTList(); $$->push_back($1); }
        | fn_list fn_def { $$ = $1; $$->push_back($2); }
;

primary_expression : TIDENTIFIER
                   | TINTEGER
                   | TSTRING
                   | TDOUBLE
                   | TCHAR
                   | '(' expression ')' { $$ = $2; }
;

postfix_expression : primary_expression
                   | postfix_expression OP_INC
                   | postfix_expression OP_DEC
                   | postfix_expression '(' ')'
                   | postfix_expression '(' arg_val_list ')'
                   | postfix_expression '[' expression ']'
                   | postfix_expression '.' TIDENTIFIER
;

unary_expression : postfix_expression
                 | '*' unary_expression { $$ = UNARY_EXPR(Deref, $2); }
                 | '!' unary_expression { $$ = UNARY_EXPR(Not, $2); }
                 | '~' unary_expression { $$ = UNARY_EXPR(Bnot, $2); }
                 | '-' unary_expression { $$ = UNARY_EXPR(Neg, $2); }
                 | OP_INC unary_expression { $$ = UNARY_EXPR(PreInc, $2); }
                 | OP_DEC unary_expression { $$ = UNARY_EXPR(PreDec, $2); }
;

infix_expression : unary_expression
                 | infix_expression TIDENTIFIER infix_expression { $$ = $1; }
                 | infix_expression '=' infix_expression { $$ = BINARY_EXPR(Eq, $1, $3); }
                 | infix_expression '+' infix_expression { $$ = BINARY_EXPR(Add, $1, $3); }
                 | infix_expression '-' infix_expression { $$ = BINARY_EXPR(Sub, $1, $3); }
                 | infix_expression '*' infix_expression { $$ = BINARY_EXPR(Mul, $1, $3); }
                 | infix_expression '/' infix_expression { $$ = BINARY_EXPR(Div, $1, $3); }
                 | infix_expression '%' infix_expression { $$ = BINARY_EXPR(Mod, $1, $3); }
                 | infix_expression '&' infix_expression { $$ = BINARY_EXPR(Band, $1, $3); }
                 | infix_expression '|' infix_expression { $$ = BINARY_EXPR(Bor, $1, $3); }
                 | infix_expression '^' infix_expression { $$ = BINARY_EXPR(Bxor, $1, $3); }
                 | infix_expression KW_OR infix_expression { $$ = BINARY_EXPR(Coalesce, $1, $3); }
;

expression : infix_expression ;


var_decl : KW_VAR TIDENTIFIER '=' expression { $$ = new Ides::AST::ASTDeclaration(Ides::AST::ASTDeclaration::DECL_VAR, $2, $4); }
         | KW_VAR TIDENTIFIER ':' TIDENTIFIER { $$ = new Ides::AST::ASTDeclaration(Ides::AST::ASTDeclaration::DECL_VAR, $2, $4); }
         | KW_VAR TIDENTIFIER ':' TIDENTIFIER '=' expression { $$ = new Ides::AST::ASTDeclaration(Ides::AST::ASTDeclaration::DECL_VAR, $2, $4, $6); }
;

val_decl : KW_VAL TIDENTIFIER '=' expression { $$ = new Ides::AST::ASTDeclaration(Ides::AST::ASTDeclaration::DECL_VAL, $2, $4); }
         | KW_VAL TIDENTIFIER ':' TIDENTIFIER  { $$ = new Ides::AST::ASTDeclaration(Ides::AST::ASTDeclaration::DECL_VAL, $2, $4); }
         | KW_VAL TIDENTIFIER ':' TIDENTIFIER '=' expression { $$ = new Ides::AST::ASTDeclaration(Ides::AST::ASTDeclaration::DECL_VAL, $2, $4, $6); }
;


arg_decl : var_decl
         | val_decl
;

arg_decl_list : arg_decl { $$ = new Ides::AST::ASTList(); $$->push_back($1); }
              | arg_decl_list ',' arg_decl { $$ = $1; $$->push_back($3); }
;

arg_val_list : expression { $$ = new Ides::AST::ASTList(); $$->push_back($1); }
             | arg_val_list ',' expression { $$ = $1; $$->push_back($1); }
;


fn_decl : KW_DEF TIDENTIFIER '(' arg_decl_list ')' { $$ = new Ides::AST::ASTFunction($2, $4, NULL); }
        | KW_DEF TIDENTIFIER '(' arg_decl_list ')' ':' TIDENTIFIER { $$ = new Ides::AST::ASTFunction($2, $4, $7); }
        | KW_DEF TIDENTIFIER '(' ')' { $$ = new Ides::AST::ASTFunction($2, NULL, NULL); }
        | KW_DEF TIDENTIFIER '(' ')' ':' TIDENTIFIER { $$ = new Ides::AST::ASTFunction($2, NULL, $6); }
;

fn_def  : fn_decl '{' expression '}' { $$ = $1; $$->val = $3; }
        | fn_decl '=' expression ';' { $$ = $1; $$->val = $3; }
;

stmt : var_decl ';'
     | val_decl ';'
;

stmt_list : stmt { $$ = new Ides::AST::ASTList(); }
          | stmt_list stmt { $$ = $1; }
;

compound_stmt : '{' stmt_list '}' ;

literal : TINTEGER
/*        | TSTRING
        | TDOUBLE
        | TCHAR */
;

%%
