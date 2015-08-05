%{
 
/*
 * Parser.y file
 * To generate the parser run: "bison Parser.y"
 */
 
#include <string>
#include "nodes.h"
#include "kuma_parser.h"
#include "kuma_lexer.h"

int yyerror(NBlock **expression, yyscan_t scanner, const char *msg) {
    printf("ERROR: %s\n", msg);
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
	NMethodCall *methodcall;
	NReturnStatement *returnstatement;
	NStatement *stmt;
	NIdentifier *ident;
	NVariableDeclaration *var_decl;
	std::vector<NVariableDeclaration*> *varvec;
	std::vector<NIdentifier*> *identvec;
	std::vector<NExpression*> *exprvec;
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
%token <token> TRETURN TEXTERN
%token <token> TFUNCDEF TEND

/* Define the type of node our nonterminal symbols represent.
   The types refer to the %union declaration above. Ex: when
   we call an ident (defined by union type ident) we are really
   calling an (NIdentifier*). It makes the compiler happy.
 */
%type <ident> ident
%type <expr> numeric expr assignment 
%type <methodcall> methodcall
%type <returnstatement> returnstmnt
%type <varvec> func_decl_args
%type <identvec> func_decl_returns multiple_assignment
%type <exprvec> call_args multiple_return
%type <block> chunk stmts
%type <stmt> stmt var_decl func_decl extern_decl
%type <token> comparison

/* Operator precedence for mathematical operators */
%left TPLUS TMINUS
%left TMUL TDIV

%start chunk

%%

chunk : stmts { *programBlock = $1; }
		;
		
stmts : stmt { $$ = new NBlock(); $$->statements.push_back($<stmt>1); }
	  | stmts stmt { $1->statements.push_back($<stmt>2); }
	  ;

stmt : var_decl | func_decl | extern_decl
	 | expr { $$ = new NExpressionStatement(*$1); }
	 | returnstmnt 
     ;

var_decl : ident ident { $$ = new NVariableDeclaration(*$1, *$2); }
		 | ident ident TEQUAL expr { $$ = new NVariableDeclaration(*$1, *$2, $4); }
		 ;

extern_decl : TEXTERN ident ident TLPAREN func_decl_args TRPAREN
                { $$ = new NExternDeclaration(*$2, *$3, *$5); delete $5; }
            ;

func_decl : TFUNCDEF ident TLPAREN func_decl_args TRPAREN TLPAREN func_decl_returns TRPAREN stmts TEND  
			{ $$ = new NFunctionDeclaration(*$2, *$4, *$7, *$9); delete $4; delete $7;}
		  ;
	
func_decl_args : /*blank*/  { $$ = new VariableList(); }
		  | var_decl { $$ = new VariableList(); $$->push_back($<var_decl>1); }
		  | func_decl_args TCOMMA var_decl { $1->push_back($<var_decl>3); }
		  ;

func_decl_returns : /*blank*/  { $$ = new IdentList(); }
		  | ident { $$ = new IdentList(); $$->push_back($<ident>1); }
		  | func_decl_returns TCOMMA ident { $1->push_back($<ident>3); }
		  ;

ident : TIDENTIFIER { $$ = new NIdentifier(*$1); delete $1; }
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
	
call_args : /*blank*/  { $$ = new ExpressionList(); }
		  | expr { $$ = new ExpressionList(); $$->push_back($1); }
		  | call_args TCOMMA expr  { $1->push_back($3); }
		  ;

comparison : TCEQ | TCNE | TCLT | TCLE | TCGT | TCGE;

%%
