%{
 
/*
 * Lexer.l file
 * To generate the lexical analyzer run: "flex Lexer.l"
 */
#include <string>
 
#include "nodes.h"
#include "kuma_parser.h"

#include <stdio.h>

#define SAVE_TOKEN  yylval->string = new std::string(yytext, yyleng); printf("String Text: %s\n", yytext);
#define TOKEN(t)    (yylval->token = t)

%}

%option outfile="kuma_lexer.cpp" header-file="kuma_lexer.h"
%option warn nodefault
%option yylineno 
%option reentrant noyywrap never-interactive nounistd
%option bison-bridge
 
LPAREN      "("
RPAREN      ")"
PLUS        "+"
MULTIPLY    "*"
 
NUMBER      [0-9]+
WS          [ \r\n\t]*
 
%%

[ \t\n]					        ;
"extern"                        return TOKEN(TEXTERN);
"return"				        return TOKEN(TRETURN);
"break"				            return TOKEN(TBREAK);
"continue"				        return TOKEN(TCONTINUE);
"class"                         return TOKEN(TCLASSDEF);
"def"                           return TOKEN(TFUNCDEF);
"end"                           return TOKEN(TEND);
"let"                           return TOKEN(TLET);
[a-zA-Z_][a-zA-Z0-9_]*          SAVE_TOKEN; return TIDENTIFIER;
\"(\\.|[^\\"])*\"               SAVE_TOKEN; return TSTRING;
[0-9]+\.[0-9]* 			        SAVE_TOKEN; return TDOUBLE;
[0-9]+					        SAVE_TOKEN; return TINTEGER;

"="						        return TOKEN(TEQUAL);
"=="				          	return TOKEN(TCEQ);
"!="			          		return TOKEN(TCNE);
"<"				          		return TOKEN(TCLT);
"<="	          				return TOKEN(TCLE);
">"				          		return TOKEN(TCGT);
">="					        return TOKEN(TCGE);

":"                             return TOKEN(TCOLON);

"("	          					return TOKEN(TLPAREN);
")"					          	return TOKEN(TRPAREN);
"{"         				    return TOKEN(TLBRACE);
"}"					          	return TOKEN(TRBRACE);

"."         					return TOKEN(TDOT);
","				          		return TOKEN(TCOMMA);

"+"				          		return TOKEN(TPLUS);
"-"		          				return TOKEN(TMINUS);
"*"		          				return TOKEN(TMUL);
"/"				          		return TOKEN(TDIV);

.                               printf("Unknown token!\n"); yyterminate();

%%
