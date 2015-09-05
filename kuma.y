%{
 
/*
 * Parser.y file
 * To generate the parser run: "bison Parser.y"
 */
 
#include <string>
#include "nodes.h"
#include "kuma_parser.h"
#include "kuma_lexer.h"

#define YYERROR_VERBOSE 1

int yyerror(NBlock **expression, yyscan_t scanner, const char *msg) 
{
    printf("ERROR: %s LINE:%d COLUMN:%d TEXT:%s\n", msg, yyget_lineno(scanner)+1, yyget_column(scanner)+1, yyget_text(scanner));
    return 0;
}
 
%}

%code requires {

#ifndef YY_TYPEDEF_YY_SCANNER_T
#define YY_TYPEDEF_YY_SCANNER_T
typedef void* yyscan_t;
#endif

}

%output  "kuma_parser.cpp"
%defines "kuma_parser.h"
 
%define api.pure
%lex-param   { yyscan_t scanner }
%parse-param { NBlock** programBlock }
%parse-param { yyscan_t scanner }

/* Represents the many different ways we can access our data */
%union {
	Node *node;
	NBlock *block;
	NExpression *expr;
	NStatement *stmt;
	NIdentifier *ident;
	std::vector<NVariableDeclaration *> *varvec;
	std::vector<NIdentifier *> *identvec;
	std::vector<NExpression *> *exprvec;
	std::string *string;
	int token;
}

/* Define our terminal symbols (tokens). This should
   match our tokens.l lex file. We also define the node type
   they represent.
 */
%token <string> TIDENTIFIER TINTEGER TDOUBLE
%token <token> TCEQ TCNE TCLT TCLE TCGT TCGE TEQUAL
%token <token> TLPAREN TRPAREN TLBRACE TRBRACE TCOMMA TDOT
%token <token> TPLUS TMINUS TMUL TDIV
%token <token> TEXTERN TSTRING TLET TCOLON
%token <token> TCLASSDEF TFUNCDEF TEND
%token <stmt> TRETURN TBREAK TCONTINUE

/* Define the type of node our nonterminal symbols represent.
   The types refer to the %union declaration above. Ex: when
   we call an ident (defined by union type ident) we are really
   calling an (NIdentifier*). It makes the compiler happy.
 */
/*%type <ident> ident
%type <expr> numeric expr assignment methodcall
%type <varvec> func_decl_args
%type <identvec> func_decl_returns multiple_assignment class_decl_bases
%type <exprvec> call_args multiple_return
%type <block> chunk stmts
%type <stmt> stmt var_decl func_decl extern_decl class_decl returnstmnt
%type <token> comparison*/

%type <ident> identifier function_declaration_return
%type <expr> expression numeric_expression multiply_expression divide_expression add_expression subtract_expression identifier_expression function_call_expression string_expression
%type <block> chunk statement_list
%type <stmt> statement flow_statement variable_declaration function_declaration function_declaration_argument assignment_statement
%type <varvec> function_declaration_argument_list
%type <identvec> function_declaration_return_list identifier_list
%type <exprvec> expression_list

/* Operator precedence for mathematical operators */
%left TPLUS TMINUS
%left TMUL TDIV
%nonassoc TEQUAL
%start chunk

%%

chunk 
    : statement_list { *programBlock = $1; }
    ;
		
statement_list 
    : statement { $$ = new NBlock(); $$->statements.push_back($<stmt>1); }
    | statement_list statement { $1->statements.push_back($<stmt>2); }
    ;

statement
    : flow_statement 
    | variable_declaration { $$ = $1; }
    | function_declaration { $$ = $1; }
    | assignment_statement
    | expression { $$ = new NExpressionStatement($1); }
    ;

flow_statement
    : TRETURN expression_list { $$ = new NReturnStatement(*$<exprvec>2); }
    ;

identifier_list
    : identifier { $$ = new IdentList(); $$->push_back($<ident>1); }
    | identifier_list TCOMMA identifier { $1->push_back($<ident>3); }
    ;

identifier
    : TIDENTIFIER { $$ = new NIdentifier(*$1); delete $1; }
    ;

variable_declaration
    : TLET identifier { $$ = new NVariableDeclaration($2); }
    | TLET identifier TCOLON identifier { $$ = new NVariableDeclaration($2, $4); }
    | TLET identifier TEQUAL expression { $$ = new NVariableDeclaration($2, $4); }
    | TLET identifier TCOLON identifier TEQUAL expression { $$ = new NVariableDeclaration($2, $4, $6); }
    ;

function_declaration
    : TFUNCDEF identifier TLPAREN function_declaration_argument_list TRPAREN TLPAREN function_declaration_return_list TRPAREN statement_list TEND
        { $$ = new NFunctionDeclaration($2, *$4, *$7, $9); delete $4; delete $7;}
    ;

function_declaration_argument_list
    : function_declaration_argument { $$ = new VariableList(); $$->push_back((NVariableDeclaration*)$<stmt>1); }
    | function_declaration_argument_list TCOMMA function_declaration_argument { $1->push_back((NVariableDeclaration*)$<stmt>3); }
    ;

function_declaration_argument
    : identifier TCOLON identifier { $$ = new NVariableDeclaration($1, $3); }
    ;

function_declaration_return_list
    : function_declaration_return { $$ = new IdentList(); $$->push_back($<ident>1); }
    | function_declaration_return_list TCOMMA function_declaration_return { $1->push_back($<ident>3); }
    ;

function_declaration_return
    : identifier { $$ = $1; }
    ;

expression_list
    : expression { $$ = new ExpressionList(); $$->push_back($<expr>1); }
    | expression_list TCOMMA expression { $1->push_back($<expr>3); }

expression
    : numeric_expression
    | string_expression
    | multiply_expression
    | divide_expression
    | add_expression
    | subtract_expression
    | identifier_expression 
    | function_call_expression
    ;

function_call_expression
    : identifier TLPAREN expression_list TRPAREN { $$ = new NMethodCall($1, *$3); delete $3; }
    ;

identifier_expression
    : identifier { $$ = $1; }
    ;

numeric_expression
    : TINTEGER { $$ = new NInteger(atol($1->c_str())); }
    | TDOUBLE { $$ = new NDouble(atof($1->c_str())); }
    ;

string_expression
    : TSTRING { $$ = new NString(*$<string>1); }
    ;

multiply_expression
    : expression TMUL expression { $$ = new NBinaryOperator($1, $2, $3); }
    ;

divide_expression
    : expression TDIV expression { $$ = new NBinaryOperator($1, $2, $3); }
    ;

add_expression
    : expression TPLUS expression { $$ = new NBinaryOperator($1, $2, $3); }
    ;

subtract_expression
    : expression TMINUS expression { $$ = new NBinaryOperator($1, $2, $3); }
    ;

assignment_statement
    : identifier_list TEQUAL expression { $$ = new NAssignment(*$<identvec>1, $3); }
    ;


/*
var_decl : ident ident { $$ = new NVariableDeclaration(*$1, *$2); }
		 | ident ident TEQUAL expr { $$ = new NVariableDeclaration(*$1, *$2, $4); }
		 ;

extern_decl : TEXTERN ident ident TLPAREN func_decl_args TRPAREN
                { $$ = new NExternDeclaration(*$2, *$3, *$5); delete $5; }
            ;

class_decl : TCLASSDEF ident TLPAREN class_decl_bases TRPAREN stmts TEND 
           { $$ = new NClassDeclaration(*$2, *$4, *$6); delete $4; }
           ;

class_decl_bases : { $$ = new IdentList(); }
                 | ident { $$ = new IdentList(); $$->push_back($<ident>1); }
                 | class_decl_bases TCOMMA ident { $1->push_back($<ident>3); }
                 ;

func_decl : TFUNCDEF ident TLPAREN func_decl_args TRPAREN TLPAREN func_decl_returns TRPAREN stmts TEND  
			{ $$ = new NFunctionDeclaration(*$2, *$4, *$7, *$9); delete $4; delete $7;}
		  ;
	
func_decl_args : { $$ = new VariableList(); }
		  | var_decl { $$ = new VariableList(); $$->push_back($<var_decl>1); }
		  | func_decl_args TCOMMA var_decl { $1->push_back($<var_decl>3); }
		  ;

func_decl_returns : { $$ = new IdentList(); }
		  | ident { $$ = new IdentList(); $$->push_back($<ident>1); }
		  | func_decl_returns TCOMMA ident { $1->push_back($<ident>3); }
		  ;

ident : TIDENTIFIER { printf("Creating ident\n"); $$ = new NIdentifier(*$1); delete $1; }
	  ;

numeric : TINTEGER { $$ = new NInteger(atol($1->c_str())); delete $1; }
		| TDOUBLE { $$ = new NDouble(atof($1->c_str())); delete $1; }
		;

returnstmnt : TRETURN multiple_return { $$ = new NReturnStatement(*$<exprvec>2); }
            ;

multiple_return : expr { $$ = new ExpressionList(); $$->push_back($<expr>1); }
                | multiple_return TCOMMA expr { $1->push_back($<expr>3); }

assignment : multiple_assignment TEQUAL methodcall { $$ = new NAssignmentMethodCall(*$<identvec>1, *$3); } 
           | multiple_assignment TEQUAL expr { $$ = new NAssignment(*$<identvec>1, *$3); } 
           ;

multiple_assignment : ident { $$ = new IdentList(); $$->push_back($<ident>1); }
                    | multiple_assignment TCOMMA ident { $1->push_back($<ident>3); }

methodcall : ident TLPAREN call_args TRPAREN { $$ = new NMethodCall(*$1, *$3); delete $3; }
           ;

expr : assignment
	 | methodcall
	 | ident { $<ident>$ = $1; }
	 | numeric
     | expr TMUL expr { $$ = new NBinaryOperator(*$1, $2, *$3); }
     | expr TDIV expr { $$ = new NBinaryOperator(*$1, $2, *$3); }
     | expr TPLUS expr { $$ = new NBinaryOperator(*$1, $2, *$3); }
     | expr TMINUS expr { $$ = new NBinaryOperator(*$1, $2, *$3); }
 	 | expr comparison expr { $$ = new NBinaryOperator(*$1, $2, *$3); }
     | TLPAREN expr TRPAREN { $$ = $2; }
	 ;
	
call_args : { $$ = new ExpressionList(); }
		  | expr { $$ = new ExpressionList(); $$->push_back($1); }
		  | call_args TCOMMA expr  { $1->push_back($3); }
		  ;

comparison : TCEQ | TCNE | TCLT | TCLE | TCGT | TCGE;*/

%%
