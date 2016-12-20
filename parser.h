#ifndef KUMA_PARSER_H
#define KUMA_PARSER_H

#include "lexer.h"
#include "ast.h"


typedef struct 
{
    char *ctx;
    const char *error;
    int in_args;
    kuma_token_t *tok;
    kuma_lexer_t *lex;
} kuma_parser_t;

int kuma_parser_init(kuma_parser_t *parser, kuma_lexer_t *lex);

kuma_block_node * kuma_parser_parse(kuma_parser_t *parser);

#endif
