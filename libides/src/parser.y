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
    #define NEW_INFIX(op, lhs, rhs) new Ides::AST::ASTInfixExpression(new Ides::AST::ASTIdentifier(op), lhs, rhs)
    #define NEW_PREFIX(op, arg) new Ides::AST::ASTUnaryExpression(Ides::AST::ASTUnaryExpression::UNARY_PREFIX, new Ides::AST::ASTIdentifier(op), arg)
    #define NEW_POSTFIX(op, arg) new Ides::AST::ASTUnaryExpression(Ides::AST::ASTUnaryExpression::UNARY_POSTFIX, new Ides::AST::ASTIdentifier(op), arg)
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
    Ides::AST::ASTIfStatement* ast_if;
    Ides::AST::ASTWhileStatement* ast_while;
    Ides::AST::ASTForStatement* ast_for;
    
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
%token KW_VOID KW_UNIT KW_BOOL KW_INT8 KW_UINT8 KW_INT16 KW_UINT16 KW_INT32 KW_UINT32 KW_INT64 KW_UINT64 KW_FLOAT32 KW_FLOAT64
%token KW_TRUE KW_FALSE

// Visibility specifiers
%token KW_PUBLIC KW_PROTECTED KW_INTERNAL KW_PRIVATE KW_EXTERN

// Keywords
%token KW_DEF KW_FN KW_VAR KW_VAL KW_NULL
%token KW_IF KW_ELSE KW_DO KW_WHILE KW_FOR
// Keyword operators
%token KW_THROW KW_NEW KW_RETURN

// Operators
%token OP_INC OP_DEC OP_COALESCE OP_CAST
%token OP_EQ OP_NE OP_LT OP_LE OP_GT OP_GE

%token <ast_int> TINTEGER
%token <ast_ident> TIDENTIFIER
%token <ast_base> TCHAR
%token <ast_istr> TSTRING
%token <ast_base> TDOUBLE

%type <ast_prog> program program_decl program_decl_list

%type <ast_fn> fn_decl fn_def extern_def
%type <ast_list> dictionary_expression array_expression
%type <ast_list> arg_decl_list arg_val_list dictionary_val_list

%type <ast_decl> var_decl val_decl arg_decl
%type <ast_type> var_type
%type <ast_list> var_type_list

%type <ast_expr> expression infix_expression prefix_expression postfix_expression primary_expression
%type <ast_stmt> stmt
%type <ast_block> stmt_list
%type <ast_if> if_stmt
%type <ast_while> while_stmt
%type <ast_for> for_stmt
%type <ast_base> for_start

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
                   | KW_TRUE { SET_EXPRLOC($$, @$); }
                   | KW_FALSE { SET_EXPRLOC($$, @$); }
                   | TSTRING { SET_EXPRLOC($$, @$); }
                   | TDOUBLE { SET_EXPRLOC($$, @$); }
                   | TCHAR { SET_EXPRLOC($$, @$); }
                   | KW_NULL { $$ = new Ides::AST::ASTNullExpr(); SET_EXPRLOC($$, @$); }
                   | KW_RETURN { $$ = new Ides::AST::ASTReturnExpression(NULL); SET_EXPRLOC($$, @$); }
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
                   //| postfix_expression operator { $$ = new Ides::AST::ASTUnaryExpression(Ides::AST::ASTUnaryExpression::UNARY_POSTFIX, $2, $1); SET_EXPRLOC($$, @$); }
                   | postfix_expression '(' ')' { $$ = new Ides::AST::ASTFunctionCall($1, NULL); SET_EXPRLOC($$, @$); }
                   | postfix_expression '(' arg_val_list ')' { $$ = new Ides::AST::ASTFunctionCall($1, $3); SET_EXPRLOC($$, @$); }
                   | postfix_expression '[' arg_val_list ']' { $$ = new Ides::AST::ASTBracketCall($1, $3); SET_EXPRLOC($$, @$); }
                   | postfix_expression '.' TIDENTIFIER
;

prefix_expression : postfix_expression
                  | '*' prefix_expression { $$ = NEW_PREFIX("*", $2); SET_EXPRLOC($$, @$); }
                  | '!' prefix_expression { $$ = NEW_PREFIX("!", $2); SET_EXPRLOC($$, @$); }
                  | '~' prefix_expression { $$ = NEW_PREFIX("~", $2); SET_EXPRLOC($$, @$); }
                  | '-' prefix_expression { $$ = NEW_PREFIX("-", $2); SET_EXPRLOC($$, @$); }
                  | "++" prefix_expression { $$ = NEW_PREFIX("++", $2); SET_EXPRLOC($$, @$); }
                  | "--" prefix_expression { $$ = NEW_PREFIX("--", $2); SET_EXPRLOC($$, @$); }
;

infix_expression : prefix_expression
                 | infix_expression '=' infix_expression { $$ = new Ides::AST::ASTAssignmentExpression($1, $3); SET_EXPRLOC($$, @$); }
                 | infix_expression '+' infix_expression { $$ = NEW_INFIX("+", $1, $3); SET_EXPRLOC($$, @$); }
                 | infix_expression '-' infix_expression { $$ = NEW_INFIX("-", $1, $3); SET_EXPRLOC($$, @$); }
                 | infix_expression '*' infix_expression { $$ = NEW_INFIX("*", $1, $3); SET_EXPRLOC($$, @$); }
                 | infix_expression '/' infix_expression { $$ = NEW_INFIX("/", $1, $3); SET_EXPRLOC($$, @$); }
                 | infix_expression '%' infix_expression { $$ = NEW_INFIX("%", $1, $3); SET_EXPRLOC($$, @$); }
                 
                 | infix_expression '&' infix_expression { $$ = NEW_INFIX("&", $1, $3); SET_EXPRLOC($$, @$); }
                 | infix_expression '|' infix_expression { $$ = NEW_INFIX("|", $1, $3); SET_EXPRLOC($$, @$); }
                 | infix_expression '^' infix_expression { $$ = NEW_INFIX("^", $1, $3); SET_EXPRLOC($$, @$); }
                 
                 | infix_expression OP_EQ infix_expression { $$ = NEW_INFIX("==", $1, $3); SET_EXPRLOC($$, @$); }
                 | infix_expression OP_NE infix_expression { $$ = NEW_INFIX("!=", $1, $3); SET_EXPRLOC($$, @$); }
                 | infix_expression OP_LT infix_expression { $$ = NEW_INFIX("<", $1, $3); SET_EXPRLOC($$, @$); }
                 | infix_expression OP_LE infix_expression { $$ = NEW_INFIX("<=", $1, $3); SET_EXPRLOC($$, @$); }
                 | infix_expression OP_GT infix_expression { $$ = NEW_INFIX(">", $1, $3); SET_EXPRLOC($$, @$); }
                 | infix_expression OP_GE infix_expression { $$ = NEW_INFIX(">=", $1, $3); SET_EXPRLOC($$, @$); }
;

expression : infix_expression
           //| KW_NEW infix_expression { $$ = NEW_POSTFIX("new", $2); SET_EXPRLOC($$, @$); }
           //| KW_THROW infix_expression { $$ = NEW_POSTFIX("throw", $2); SET_EXPRLOC($$, @$); }
           | KW_RETURN infix_expression { $$ = new Ides::AST::ASTReturnExpression($2); SET_EXPRLOC($$, @$); }
;

var_type : '(' var_type ')' { $$ = $2; SET_EXPRLOC($$, @$); }
         | TIDENTIFIER { $$ = new Ides::AST::ASTTypeName($1); SET_EXPRLOC($$, @$); }

         | KW_VOID   { $$ = new Ides::AST::ASTVoidType(); SET_EXPRLOC($$, @$); }
         | KW_UNIT   { $$ = new Ides::AST::ASTUnitType(); SET_EXPRLOC($$, @$); }
         
         | KW_BOOL   { $$ = new Ides::AST::ASTInteger1Type(); SET_EXPRLOC($$, @$); }
         | KW_INT8   { $$ = new Ides::AST::ASTInteger8Type();  SET_EXPRLOC($$, @$); }
         | KW_UINT8  { $$ = new Ides::AST::ASTUInteger8Type();  SET_EXPRLOC($$, @$); }
         | KW_INT16  { $$ = new Ides::AST::ASTInteger16Type(); SET_EXPRLOC($$, @$); }
         | KW_UINT16 { $$ = new Ides::AST::ASTUInteger16Type(); SET_EXPRLOC($$, @$); }
         | KW_INT32  { $$ = new Ides::AST::ASTInteger32Type(); SET_EXPRLOC($$, @$); }
         | KW_UINT32 { $$ = new Ides::AST::ASTUInteger32Type(); SET_EXPRLOC($$, @$); }
         | KW_INT64  { $$ = new Ides::AST::ASTInteger64Type(); SET_EXPRLOC($$, @$); }
         | KW_UINT64 { $$ = new Ides::AST::ASTUInteger64Type(); SET_EXPRLOC($$, @$); }

         | KW_FLOAT32 { $$ = new Ides::AST::ASTFloat32Type(); SET_EXPRLOC($$, @$); }
         | KW_FLOAT64 { $$ = new Ides::AST::ASTFloat64Type(); SET_EXPRLOC($$, @$); }
         
         | KW_FN '(' ')' ':' var_type { $$ = new Ides::AST::ASTFunctionType(NULL, $5); SET_EXPRLOC($$, @$); }
         | KW_FN '(' var_type_list ')' ':' var_type { $$ = new Ides::AST::ASTFunctionType($3, $6); SET_EXPRLOC($$, @$); }
         | KW_FN '(' ')' { $$ = new Ides::AST::ASTFunctionType(NULL, NULL); SET_EXPRLOC($$, @$); }
         | KW_FN '(' var_type_list ')' { $$ = new Ides::AST::ASTFunctionType($3, NULL); SET_EXPRLOC($$, @$); }
         
         | var_type '*' { $$ = new Ides::AST::ASTPtrType($1); SET_EXPRLOC($$, @$); }
;

var_type_list : var_type { $$ = new Ides::AST::ASTList(); $$->push_back($1); SET_EXPRLOC($$, @$); }
              | var_type_list ',' var_type { $$ = $1; $$->push_back($3); SET_EXPRLOC($$, @$); }
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

if_stmt : KW_IF '(' expression ')' stmt KW_ELSE stmt { $$ = new Ides::AST::ASTIfStatement($3, $5, $7); SET_EXPRLOC($$, @$); }
        | KW_IF '(' expression ')' stmt { $$ = new Ides::AST::ASTIfStatement($3, $5, NULL); SET_EXPRLOC($$, @$); }
;

while_stmt : KW_WHILE '(' expression ')' stmt { $$ = new Ides::AST::ASTWhileStatement($3, $5); SET_EXPRLOC($$, @$); }

for_stmt : KW_FOR '(' for_start ';' expression ';' expression ')' stmt { $$ = new Ides::AST::ASTForStatement($3, $5, $7, $9); SET_EXPRLOC($$, @$); }

for_start : expression
          | var_decl
;

stmt : var_decl ';' { SET_EXPRLOC($$, @$); }
     | val_decl ';' { SET_EXPRLOC($$, @$); }
     | expression ';' { SET_EXPRLOC($$, @$); }
     | if_stmt { SET_EXPRLOC($$, @$); }
     | while_stmt { SET_EXPRLOC($$, @$); }
     | for_stmt { SET_EXPRLOC($$, @$); }
     | '{' stmt_list '}' { $$ = (Ides::AST::ASTStatement*)$2; SET_EXPRLOC($$, @$); }
;

stmt_list : stmt { $$ = new Ides::AST::ASTCompoundStatement(); $$->push_back($1); SET_EXPRLOC($$, @$); }
          | stmt_list stmt { $$ = $1; $$->push_back($2); SET_EXPRLOC($$, @$); }
          | stmt_list ';' { $$ = $1; SET_EXPRLOC($$, @$); }
;

%%
