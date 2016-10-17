#ifndef KUMA_LEXER_H
#define KUMA_LEXER_H

#include "stdio.h"

enum TokenType
{
    TOK_EOF = 258,

    TOK_EXTERN,
    TOK_RETURN,
    TOK_BREAK,
    TOK_CONTINUE,
    TOK_CLASS,
    TOK_DEF,
    TOK_END,
    TOK_LET,
    TOK_VAR,
    
    TOK_IDENTIFIER,
    TOK_STRING,
    TOK_NUMBER,
    TOK_INTEGER,

    TOK_EQUAL,    
    TOK_NOT,
    TOK_CEQ,    
    TOK_CNE,
    TOK_CLT,
    TOK_CLE,
    TOK_CGT,
    TOK_CGE,

    TOK_COLON,
    TOK_LPAREN,
    TOK_RPAREN,
    TOK_LBRACE,
    TOK_RBRACE,

    TOK_DOT,
    TOK_COMMA,

    TOK_PLUS,
    TOK_MINUS,
    TOK_MUL,
    TOK_DIV,

    TOK_NEWLINE,

    TOK_ILLEGAL,
};

typedef struct
{
    int type;
    const char* name;

    union
    {
        char* string;
        double number;
        int integer;
    } value;

} kuma_token_t;

typedef struct 
{
    const char* filename;
    char* source;
    off_t cursor;
    char last;

    int lineno;
    const char* error;

    kuma_token_t tok;

} kuma_lexer_t;

int kuma_lexer_init(kuma_lexer_t *lex, char *source, const char *filename);
int kuma_lexer_scan(kuma_lexer_t *lex);
int kuma_lexer_dump(kuma_lexer_t *lex);

#endif
