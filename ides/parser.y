%{
    #include <iostream>
    #include <ides/Parsing/ParserCommon.h>
    #include <stdexcept>
    #include <sstream>
    
    void yyerror(YYLTYPE* locp, Ides::Parsing::ParseContext* context, Ides::AST::AST** output, const char* err)
	{
        //throw Ides::Diagnostics::CompileError(err, *locp);
        std::cerr << err << std::endl;
        exit(1);
	}
    
    #define SET_EXPRLOC(x, loc) (x)->exprloc = (loc)
    #define NEW_INFIX(op, lhs, rhs) new Ides::AST::InfixExpression(new Ides::AST::Token(op), lhs, rhs)
    #define NEW_PREFIX(op, arg) new Ides::AST::UnaryExpression(Ides::AST::UnaryExpression::UNARY_PREFIX, new Ides::AST::Token(op), arg)
    #define NEW_POSTFIX(op, arg) new Ides::AST::UnaryExpression(Ides::AST::UnaryExpression::UNARY_POSTFIX, new Ides::AST::Token(op), arg)
%}

%pure-parser
%defines
%error-verbose
%locations
%debug


%parse-param { Ides::Parsing::ParseContext* context }
%parse-param { Ides::AST::AST** output }
%lex-param { void* scanner  }

%union {
    /***** AST Elements *****/
    Ides::AST::AST* ast_base;
    Ides::AST::Token* ast_ident;
    
    Ides::AST::ASTList* ast_list;
    Ides::AST::ExpressionList* expr_list;
    Ides::AST::DeclarationList* decl_list;
    Ides::AST::VariableDeclarationList* var_decl_list;
    Ides::AST::TypeList* type_list;
    
    Ides::AST::CompilationUnit* ast_prog;
    
    Ides::AST::Type* ast_type;
    
    Ides::AST::Expression* ast_expr;
    Ides::AST::Statement* ast_stmt;
    Ides::AST::Declaration* ast_decl;
    
    Ides::AST::VariableDeclaration* var_decl;
    
    Ides::AST::FunctionDeclaration* ast_fn;
    
    Ides::AST::Block* ast_block;
    
    Ides::AST::StructDeclaration* ast_struct;
    //Ides::AST::ClassDeclaration* ast_class;
    
    Ides::AST::ConstantExpression* ast_const;
    Ides::AST::ConstantStringExpression* ast_str;
    
    /***** Type Elements *****/
    Ides::Types::Type* type_base;
    
    Ides::AST::Specifier spec;
}

%{
    
    int yylex(YYSTYPE* lvalp, YYLTYPE* llocp, void* scan);
    
    #define scanner context->GetScanner()
    
%}

// Builtin types
%token KW_VOID KW_UNIT KW_BOOL KW_INT8 KW_UINT8 KW_INT16 KW_UINT16 KW_INT32 KW_UINT32 KW_INT64 KW_UINT64 KW_FLOAT32 KW_FLOAT64
%token KW_TRUE KW_FALSE

// Specifiers
%token KW_PUBLIC KW_PROTECTED KW_INTERNAL KW_PRIVATE KW_EXTERN KW_CONST

// Keywords
%token KW_DEF KW_FN KW_STRUCT KW_CLASS KW_VAR KW_VAL KW_NULL KW_NAMESPACE
%token KW_IF KW_ELSE KW_DO KW_WHILE KW_FOR
// Keyword operators
%token KW_THROW KW_NEW KW_RETURN

// Operators
%token OP_INC OP_DEC OP_COALESCE OP_CAST
%token OP_EQ OP_NE OP_LT OP_LE OP_GT OP_GE

%token <ast_int> TINTEGER
%token <ast_ident> TIDENTIFIER
%token <ast_base> TCHAR
%token <ast_str> TSTRING
%token <ast_base> TDOUBLE

%type <ast_prog> program program_decl_list
%type <ast_decl> program_decl member_decl

%type <ast_fn> fn_decl fn_def extern_def
//%type <ast_list> dictionary_expression dictionary_val_list array_expression
%type <expr_list> arg_val_list
%type <decl_list> member_decl_list
%type <var_decl_list> arg_decl_list

%type <var_decl> var_decl val_decl arg_decl
%type <ast_type> var_type
%type <type_list> var_type_list

%type <ast_struct> struct_def
//%type <ast_class> class_def

%type <ast_expr> expression infix_expression prefix_expression postfix_expression primary_expression
%type <ast_stmt> stmt if_stmt while_stmt for_stmt for_start
%type <ast_block> stmt_list

%type <ast_prog> root

%type <spec> specifier


/* Operator precedence for mathematical operators */
%nonassoc KW_THROW KW_RETURN
%left '='
%left OP_COALESCE
%left OP_EQ OP_NE
%left OP_LT OP_LE OP_GT OP_GE
%left '+' '-'
%left '*' '/' '%'
%right OP_CAST KW_NEW 

%start root

%%

root : program { *output = $1; }
;

program : program_decl_list
;

program_decl : member_decl
;

program_decl_list : program_decl { $$ = new Ides::AST::CompilationUnit(); $$->push_back(boost::shared_ptr<Ides::AST::Declaration>($1)); }
                  | program_decl_list program_decl { $$ = $1; $$->push_back(boost::shared_ptr<Ides::AST::Declaration>($2)); }
;

primary_expression : TIDENTIFIER { SET_EXPRLOC($$, @$); }
                   | TINTEGER { SET_EXPRLOC($$, @$); }
                   | KW_TRUE { SET_EXPRLOC($$, @$); }
                   | KW_FALSE { SET_EXPRLOC($$, @$); }
                   | TSTRING { SET_EXPRLOC($$, @$); }
                   | TDOUBLE { SET_EXPRLOC($$, @$); }
                   | TCHAR { SET_EXPRLOC($$, @$); }
                   | KW_NULL { $$ = Ides::AST::NullExpression::Create(); SET_EXPRLOC($$, @$); }
                   | KW_RETURN { $$ = Ides::AST::ReturnExpression::Create(); SET_EXPRLOC($$, @$); }
                   //| array_expression
                   //| dictionary_expression
                   | '(' expression ')' { $$ = $2; }
;
/*
array_expression : '[' arg_val_list ']' { $$ = $2; }
;

dictionary_expression : '{' dictionary_val_list '}' { $$ = $2; }
;

dictionary_val_list : expression ':' expression { $$ = new Ides::AST::ASTDictExpression(); $$->push_back(BINARY_EXPR(DictPair, $1, $3)); }
                    | dictionary_val_list ',' expression ':' expression { $$ = $1; $$->push_back(BINARY_EXPR(DictPair, $3, $5)); }
;*/

postfix_expression : primary_expression
                   //| postfix_expression operator { $$ = new Ides::AST::ASTUnaryExpression(Ides::AST::ASTUnaryExpression::UNARY_POSTFIX, $2, $1); SET_EXPRLOC($$, @$); }
                   | postfix_expression '(' ')' { $$ = Ides::AST::FunctionCallExpression::Create($1); SET_EXPRLOC($$, @$); }
                   | postfix_expression '(' arg_val_list ')' { $$ = Ides::AST::FunctionCallExpression::Create($1, $3); SET_EXPRLOC($$, @$); }
                   //| postfix_expression '[' arg_val_list ']' { $$ = new Ides::AST::BracketCall($1, $3); SET_EXPRLOC($$, @$); }
                   //| postfix_expression '.' TIDENTIFIER { $$ = new Ides::AST::DotExpression($1, $3); SET_EXPRLOC($$, @$); }
;

prefix_expression : postfix_expression
                  //| '*' prefix_expression { $$ = new Ides::AST::DereferenceExpression($2); SET_EXPRLOC($$, @$); }
                  //| '&' prefix_expression { $$ = new Ides::AST::AddressOfExpression($2); SET_EXPRLOC($$, @$); }
                  | '!' prefix_expression { $$ = NEW_PREFIX("!", $2); SET_EXPRLOC($$, @$); }
                  | '~' prefix_expression { $$ = NEW_PREFIX("~", $2); SET_EXPRLOC($$, @$); }
                  | '-' prefix_expression { $$ = NEW_PREFIX("-", $2); SET_EXPRLOC($$, @$); }
                  | "++" prefix_expression { $$ = NEW_PREFIX("++", $2); SET_EXPRLOC($$, @$); }
                  | "--" prefix_expression { $$ = NEW_PREFIX("--", $2); SET_EXPRLOC($$, @$); }
;

infix_expression : prefix_expression
                 | infix_expression '=' infix_expression { $$ = new Ides::AST::AssignmentExpression($1, $3); SET_EXPRLOC($$, @$); }
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
                 
                 //| infix_expression OP_CAST var_type { $$ = new Ides::AST::CastExpression($1, $3); SET_EXPRLOC($$, @$); }
;

expression : infix_expression
           //| KW_NEW infix_expression { $$ = NEW_POSTFIX("new", $2); SET_EXPRLOC($$, @$); }
           //| KW_THROW infix_expression { $$ = NEW_POSTFIX("throw", $2); SET_EXPRLOC($$, @$); }
           | KW_RETURN infix_expression { $$ = Ides::AST::ReturnExpression::Create($2); SET_EXPRLOC($$, @$); }
;

specifier : KW_PUBLIC { $$ = Ides::AST::PUBLIC; }
          | KW_PROTECTED { $$ = Ides::AST::PROTECTED; }
          | KW_INTERNAL { $$ = Ides::AST::INTERNAL; }
          | KW_PRIVATE { $$ = Ides::AST::PRIVATE; }
;

var_type : var_type '*' { $$ = new Ides::AST::PtrType($1); SET_EXPRLOC($$, @$); }

         | KW_VOID   { $$ = new Ides::AST::VoidType(); SET_EXPRLOC($$, @$); }
         | KW_UNIT   { $$ = new Ides::AST::UnitType(); SET_EXPRLOC($$, @$); }
         
         | KW_BOOL   { $$ = new Ides::AST::BoolType(); SET_EXPRLOC($$, @$); }

         | KW_INT8   { $$ = new Ides::AST::Integer8Type();  SET_EXPRLOC($$, @$); }
         | KW_UINT8  { $$ = new Ides::AST::UInteger8Type();  SET_EXPRLOC($$, @$); }
         | KW_INT16  { $$ = new Ides::AST::Integer16Type(); SET_EXPRLOC($$, @$); }
         | KW_UINT16 { $$ = new Ides::AST::UInteger16Type(); SET_EXPRLOC($$, @$); }
         | KW_INT32  { $$ = new Ides::AST::Integer32Type(); SET_EXPRLOC($$, @$); }
         | KW_UINT32 { $$ = new Ides::AST::UInteger32Type(); SET_EXPRLOC($$, @$); }
         | KW_INT64  { $$ = new Ides::AST::Integer64Type(); SET_EXPRLOC($$, @$); }
         | KW_UINT64 { $$ = new Ides::AST::UInteger64Type(); SET_EXPRLOC($$, @$); }

         | KW_FLOAT32 { $$ = new Ides::AST::Float32Type(); SET_EXPRLOC($$, @$); }
         | KW_FLOAT64 { $$ = new Ides::AST::Float64Type(); SET_EXPRLOC($$, @$); }
         
         | KW_FN '(' ')' ':' var_type { $$ = new Ides::AST::FunctionType(NULL, $5); SET_EXPRLOC($$, @$); }
         | KW_FN '(' var_type_list ')' ':' var_type { $$ = new Ides::AST::FunctionType($3, $6); SET_EXPRLOC($$, @$); }
         | KW_FN '(' ')' { $$ = new Ides::AST::FunctionType(NULL, NULL); SET_EXPRLOC($$, @$); }
         | KW_FN '(' var_type_list ')' { $$ = new Ides::AST::FunctionType($3, NULL); SET_EXPRLOC($$, @$); }
         
         | TIDENTIFIER { $$ = new Ides::AST::TypeName($1); SET_EXPRLOC($$, @$); }

         //| KW_CONST var_type { $$ = $2; $$->SetConst(true); }
;

var_type_list : var_type { $$ = new Ides::AST::TypeList(); $$->push_back(boost::shared_ptr<Ides::AST::Type>($1)); }
              | var_type_list ',' var_type { $$ = $1; $$->push_back(boost::shared_ptr<Ides::AST::Type>($3)); }
;

var_decl : KW_VAR TIDENTIFIER '=' expression { $$ = new Ides::AST::VariableDeclaration(Ides::AST::VariableDeclaration::DECL_VAR, $2, $4); SET_EXPRLOC($$, @$); }
         | KW_VAR TIDENTIFIER ':' var_type { $$ = new Ides::AST::VariableDeclaration(Ides::AST::VariableDeclaration::DECL_VAR, $2, $4); SET_EXPRLOC($$, @$); }
         | KW_VAR TIDENTIFIER ':' var_type '=' expression { $$ = new Ides::AST::VariableDeclaration(Ides::AST::VariableDeclaration::DECL_VAR, $2, $4, $6); SET_EXPRLOC($$, @$); }
;

val_decl : KW_VAL TIDENTIFIER '=' expression
                {
                    $$ = new Ides::AST::VariableDeclaration(Ides::AST::VariableDeclaration::DECL_VAL, $2, $4);
                    SET_EXPRLOC($$, @$);
                }
         | KW_VAL TIDENTIFIER ':' var_type
                {
                    $$ = new Ides::AST::VariableDeclaration(Ides::AST::VariableDeclaration::DECL_VAL, $2, $4);
                    SET_EXPRLOC($$, @$);
                }
         | KW_VAL TIDENTIFIER ':' var_type '=' expression
                {
                    $$ = new Ides::AST::VariableDeclaration(Ides::AST::VariableDeclaration::DECL_VAL, $2, $4, $6);
                    SET_EXPRLOC($$, @$);
                }
;


arg_decl : var_decl
         | val_decl
;

arg_decl_list : arg_decl { $$ = new Ides::AST::VariableDeclarationList(); $$->push_back(boost::shared_ptr<Ides::AST::VariableDeclaration>($1)); }
              | arg_decl_list ',' arg_decl { $$ = $1; $$->push_back(boost::shared_ptr<Ides::AST::VariableDeclaration>($3)); }
;

arg_val_list : expression { $$ = new Ides::AST::ExpressionList(); $$->push_back(boost::shared_ptr<Ides::AST::Expression>($1)); }
             | arg_val_list ',' expression { $$ = $1; $$->push_back(boost::shared_ptr<Ides::AST::Expression>($3)); }
;


fn_decl : KW_DEF TIDENTIFIER '(' arg_decl_list ')' { $$ = new Ides::AST::FunctionDeclaration($2, $4, NULL); SET_EXPRLOC($$, @$); }
        | KW_DEF TIDENTIFIER '(' arg_decl_list ')' ':' var_type { $$ = new Ides::AST::FunctionDeclaration($2, $4, $7); SET_EXPRLOC($$, @$); }
        | KW_DEF TIDENTIFIER '(' ')' { $$ = new Ides::AST::FunctionDeclaration($2, NULL, NULL); SET_EXPRLOC($$, @$); }
        | KW_DEF TIDENTIFIER '(' ')' ':' var_type { $$ = new Ides::AST::FunctionDeclaration($2, NULL, $6); SET_EXPRLOC($$, @$); }
;

fn_def  : fn_decl '{' stmt_list '}' { $$ = $1; $$->body = $3; }
        | fn_decl '{' '}'
        | fn_decl '=' expression ';' { $$ = $1; $$->val = $3; }
;

member_decl : fn_def
            | extern_def
            //| class_def
            | struct_def
            | var_decl ';'
            | val_decl ';'
;

member_decl_list : member_decl { $$ = new Ides::AST::DeclarationList(); $$->push_back(boost::shared_ptr<Ides::AST::Declaration>($1)); }
                 | member_decl_list member_decl { $$ = $1; $$->push_back(boost::shared_ptr<Ides::AST::Declaration>($2)); }
;

struct_def : KW_STRUCT TIDENTIFIER '{' member_decl_list '}' { $$ = new Ides::AST::StructDeclaration($2, $4); SET_EXPRLOC($$, @$); }
           | KW_STRUCT TIDENTIFIER '{' '}' { $$ = new Ides::AST::StructDeclaration($2, NULL); SET_EXPRLOC($$, @$); }
;
/*
class_def : KW_CLASS TIDENTIFIER '{' member_decl_list '}' { $$ = new Ides::AST::ASTClass($4); SET_EXPRLOC($$, @$); }
          | KW_CLASS TIDENTIFIER '{' '}' { $$ = new Ides::AST::ASTClass(NULL); SET_EXPRLOC($$, @$); }
;*/

extern_def : KW_EXTERN fn_decl ';' { $$ = $2; }
;

if_stmt : KW_IF '(' expression ')' stmt KW_ELSE stmt { $$ = new Ides::AST::IfStatement($3, $5, $7); SET_EXPRLOC($$, @$); }
        | KW_IF '(' expression ')' stmt { $$ = new Ides::AST::IfStatement($3, $5, NULL); SET_EXPRLOC($$, @$); }
;

while_stmt : KW_WHILE '(' expression ')' stmt { $$ = new Ides::AST::WhileStatement($3, $5); SET_EXPRLOC($$, @$); }

for_stmt : KW_FOR '(' for_start ';' expression ';' expression ')' stmt { $$ = new Ides::AST::ForStatement($3, $5, $7, $9); SET_EXPRLOC($$, @$); }

for_start : expression
          | var_decl
;

stmt : var_decl ';' { SET_EXPRLOC($$, @$); }
     | val_decl ';' { SET_EXPRLOC($$, @$); }
     | expression ';' { SET_EXPRLOC($$, @$); }
     | if_stmt { SET_EXPRLOC($$, @$); }
     | while_stmt { SET_EXPRLOC($$, @$); }
     | for_stmt { SET_EXPRLOC($$, @$); }
     | '{' stmt_list '}' { $$ = (Ides::AST::Statement*)$2; SET_EXPRLOC($$, @$); }
;

stmt_list : stmt { $$ = new Ides::AST::Block(); $$->statements.push_back(boost::shared_ptr<Ides::AST::Statement>($1)); SET_EXPRLOC($$, @$); }
          | stmt_list stmt { $$ = $1; $$->statements.push_back(boost::shared_ptr<Ides::AST::Statement>($2)); SET_EXPRLOC($$, @$); }
          | stmt_list ';' { $$ = $1; SET_EXPRLOC($$, @$); }
;

%%
