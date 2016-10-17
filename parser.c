#include "parser.h"
#include "lexer.h"

#define NEXT (parser->tok = (kuma_lexer_scan(parser->lex), &(parser->lex->tok)))

#define IS(t) (parser->tok->type == t)

#define ACCEPT(t) (IS(t) && NEXT)

#define LINENO parser->lex->lineno

int kuma_parser_init(kuma_parser_t *parser, kuma_lexer_t *lex)
{
    parser->lex = lex;
    parser->tok = NULL;
    parser->ctx = NULL;
    parser->error = NULL;
}

kuma_node * parse_type(kuma_parser_t *parser)
{
    if(!IS(TOK_IDENTIFIER))
        return NULL;

    kuma_node *node = (kuma_node *)kuma_ident_node_new(LINENO, parser->tok->value.string);

    NEXT;

    return node;
}

kuma_node * parse_expr(kuma_parser_t *parser)
{
    NEXT;
    return NULL;
}

// 'var' id ':' type_expr '=' expr
kuma_node * parse_var_statement(kuma_parser_t *parser)
{
    if(!ACCEPT(TOK_VAR))
        return NULL;

    if(!IS(TOK_IDENTIFIER))
        return NULL;

    const char *name = parser->tok->value.string;

    NEXT;

    if(!ACCEPT(TOK_COLON))
        return NULL;

    kuma_node * type;
    type = parse_type(parser);

    if(!ACCEPT(TOK_EQUAL))
        return NULL;

    kuma_node * expr;
    expr = parse_expr(parser);

    kuma_node * node = (kuma_node *)kuma_var_node_new(LINENO, name, type, expr);

    return node;
}

kuma_node * parse_statement(kuma_parser_t *parser)
{
    if(IS(TOK_VAR))
        return parse_var_statement(parser);

    return NULL;
}

kuma_block_node * parse_program(kuma_parser_t *parser)
{
    kuma_node *node;
    kuma_block_node *block = kuma_block_node_new(LINENO);

    NEXT;

    while (!IS(TOK_EOF))
    {
        node = parse_statement(parser);
        if (node)
        {
            ACCEPT(TOK_NEWLINE);
            klist_add(block->stmts, node);
        }
        else
        {
            return NULL;
        }
    }

    return block;
}

kuma_block_node * kuma_parser_parse(kuma_parser_t *parser)
{
    return parse_program(parser);
}
