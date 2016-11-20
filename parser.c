#include "parser.h"
#include "lexer.h"

#define NEXT (parser->tok = (kuma_lexer_scan(parser->lex), &(parser->lex->tok)))

#define IS(t) (parser->tok->type == t)

#define ACCEPT(t) (IS(t) && NEXT)

#define CURRENT parser->lex->tok.type

#define LINENO parser->lex->lineno


kuma_node * parse_expr(kuma_parser_t *parser);

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

kuma_node * parse_integer(kuma_parser_t *parser)
{
    int val = parser->tok->value.integer;
    kuma_node *node = (kuma_node *)kuma_integer_node_new(LINENO, val);

    // Consume Integer
    NEXT;

    return node;
}

kuma_node * parse_ident_expr(kuma_parser_t *parser)
{
    if(!IS(TOK_IDENTIFIER))
        return NULL;

    kuma_node *node = (kuma_node *)kuma_ident_node_new(LINENO, parser->tok->value.string);

    // Consume Identifier
    NEXT;

    return node;
}

// 'var' id ':' type_expr '=' expr
kuma_node * parse_var_expr(kuma_parser_t *parser)
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

// 'var' id ':' type_expr '=' expr
kuma_node * parse_let_expr(kuma_parser_t *parser)
{
    if(!ACCEPT(TOK_LET))
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

    kuma_node * node = (kuma_node *)kuma_let_node_new(LINENO, name, type, expr);

    return node;
}

kuma_node * parse_primary_expr(kuma_parser_t *parser)
{
    if(IS(TOK_IDENTIFIER))
        return parse_ident_expr(parser);

    if(IS(TOK_INTEGER))
        return parse_integer(parser);

    if(IS(TOK_VAR))
        return parse_var_expr(parser);

    if(IS(TOK_LET))
        return parse_let_expr(parser);

    return NULL;
}

/*
        primary
    |   '!' unary

 */
kuma_node * parse_unary_expr(kuma_parser_t *parser)
{
    return parse_primary_expr(parser);

    return NULL;
}

kuma_node * parse_binop_rhs(kuma_parser_t *parser, kuma_node *lhs)
{
    if(!IS(TOK_PLUS))
        return lhs;

    int binop = CURRENT;
    NEXT;

    kuma_node *rhs = parse_primary_expr(parser);

    // Expected RHS expression
    if(rhs == NULL)
    {
        return NULL;
    }

    kuma_node *node = (kuma_node *)kuma_binop_node_new(LINENO, binop, lhs, rhs);

    return node;
}

kuma_node * parse_expr(kuma_parser_t *parser)
{
    kuma_node *lhs = parse_unary_expr(parser);

    if(lhs == NULL)
    {
        return NULL;
    }

    return parse_binop_rhs(parser, lhs);

    //NEXT;
}



kuma_node * parse_stmnt(kuma_parser_t *parser)
{
    if(IS(TOK_VAR))
        return parse_var_expr(parser);

    if(IS(TOK_LET))
        return parse_let_expr(parser);

    return parse_expr(parser);
}

kuma_block_node * parse_program(kuma_parser_t *parser)
{
    kuma_node *node;
    kuma_block_node *block = kuma_block_node_new(LINENO);

    NEXT;

    while (!IS(TOK_EOF))
    {
        node = parse_stmnt(parser);
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
