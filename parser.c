#include "parser.h"
#include "lexer.h"

#define NEXT (parser->tok = (kuma_lexer_scan(parser->lex), &(parser->lex->tok)))

#define IS(t) (parser->tok->type == t)

#define ACCEPT(t) (IS(t) && NEXT)

#define CURRENT parser->lex->tok.type

#define LINENO parser->lex->lineno


kuma_node * parse_expr(kuma_parser_t *parser);
kuma_node * parse_block(kuma_parser_t *parser);
kuma_node * parse_if_expr(kuma_parser_t *parser);
kuma_node * parse_ident_expr(kuma_parser_t *parser);

int get_token_precedence(int token)
{
    switch(token)
    {
        case TOK_CLT:
        case TOK_CGT:
        case TOK_CEQ:
        case TOK_CGE:
        case TOK_CLE:
            return 10;
        case TOK_PLUS:
        case TOK_MINUS:
            return 20;
        case TOK_MUL:
        case TOK_DIV:
            return 40;
    }

    return -1;
}

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

kuma_node * parse_assignment_expr(kuma_parser_t *parser, char *ident)
{
    kuma_node *expr = parse_expr(parser);
    kuma_node *node = (kuma_node *)kuma_assignment_node_new(LINENO, ident, expr);
    return node;
}

kuma_node * parse_param(kuma_parser_t *parser)
{
    // Param name
    char *name = parser->tok->value.string;
    NEXT; //Consume Ident

    if(!ACCEPT(TOK_COLON))
    {
        // Colon expected
        return NULL;
    }

    // Param type
    char *type = parser->tok->value.string;
    NEXT; //Consume Ident

    kuma_node *node = (kuma_node *)kuma_param_node_new(LINENO, name, type);
    return node;
}

kuma_node * parse_function_expr(kuma_parser_t *parser)
{
    int param_count = 0;
    int return_count = 0;

    kuma_node *param_list[KUMA_MAX_PARAMS];
    kuma_node *return_list[KUMA_MAX_RETURNS];

    // Consume 'func'
    ACCEPT(TOK_FUNC);

    // Ident of Func
    char *ident = parser->tok->value.string;
    NEXT; //Consume Ident

    if(!ACCEPT(TOK_LPAREN))
    {
        // Missing LPAREN
        return NULL;
    }

    while(1)
    {
        if(ACCEPT(TOK_RPAREN))
        {
            break;
        }
        else if(ACCEPT(TOK_COMMA))
        {
            continue;
        }

        kuma_node *param = parse_param(parser);

        if(param != NULL)
        {
            param_list[param_count] = param;
            param_count++;
        }
    }

    // If returns
    if(ACCEPT(TOK_LPAREN))
    {
        while(1)
        {
            if(ACCEPT(TOK_RPAREN))
            {
                break;
            }
            else if(ACCEPT(TOK_COMMA))
            {
                continue;
            }

            kuma_node *r = parse_ident_expr(parser);

            if(r != NULL)
            {
                return_list[return_count] = r;
                return_count++;
            }
        }
    }

    // Body
    kuma_node *body = parse_block(parser);

    if(!ACCEPT(TOK_END))
    {
        return NULL;
    }

    kuma_node *node = (kuma_node *)kuma_function_node_new(LINENO, ident, param_count, param_list, return_count, return_list, body);
    return node;
}

kuma_node * parse_call_expr(kuma_parser_t *parser, char *ident)
{
    int expr_count = 0;
    kuma_node *expr_list[KUMA_MAX_PARAMS];

    kuma_node *expr = NULL;

    while(1)
    {
        if(ACCEPT(TOK_RPAREN))
        {
            break;
        }
        else if(ACCEPT(TOK_COMMA))
        {
            continue;
        }

        expr = parse_expr(parser);

        if(expr != NULL)
        {
            expr_list[expr_count] = expr;
            expr_count++;
        }
        else
        {
            // Invalid expr
            return NULL;
        }
    }

    kuma_node *node = (kuma_node *)kuma_call_node_new(LINENO, ident, expr_count, expr_list);
    return node;
}

kuma_node * parse_ident_expr(kuma_parser_t *parser)
{
    char *ident = parser->tok->value.string;
    NEXT; //Consume Ident

    if(ACCEPT(TOK_LPAREN))
    {
        return parse_call_expr(parser, ident);
    }

    // Is not comparison
    if(ACCEPT(TOK_EQUAL))
    {
        return parse_assignment_expr(parser, ident);
    }

    kuma_node *node = (kuma_node *)kuma_ident_node_new(LINENO, parser->tok->value.string);

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

kuma_node * parse_paren_expr(kuma_parser_t *parser)
{
    NEXT; // Consume LPAREN

    kuma_node *node = parse_expr(parser);

    if(node == NULL)
        return NULL;

    if(!ACCEPT(TOK_RPAREN))
    {
        // Expected ')'
        return NULL;
    }

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

    if(IS(TOK_IF))
        return parse_if_expr(parser);

    if(IS(TOK_LPAREN))
        return parse_paren_expr(parser);

    if(IS(TOK_FUNC))
        return parse_function_expr(parser);

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

kuma_node * parse_binop_rhs(kuma_parser_t *parser, kuma_node *lhs, int exp_prec)
{
    while(1)
    {
        int tok_prec = get_token_precedence(CURRENT);

        if (tok_prec < exp_prec)
            return lhs;

        int binop = CURRENT;
        NEXT; // Eat Op token

        kuma_node *rhs = parse_primary_expr(parser);

        // Expected RHS expression
        if (rhs == NULL)
        {
            return NULL;
        }

        int next_prec = get_token_precedence(CURRENT);

        if (tok_prec < next_prec)
        {
            rhs = parse_binop_rhs(parser, rhs, tok_prec + 1);

            // Expected RHS expression
            if(rhs == NULL)
            {
                return NULL;
            }
        }

        lhs = (kuma_node *)kuma_binop_node_new(LINENO, binop, lhs, rhs);
    }
}

kuma_node * parse_expr(kuma_parser_t *parser)
{
    kuma_node *lhs = parse_unary_expr(parser);

    if(lhs == NULL)
    {
        return NULL;
    }

    return parse_binop_rhs(parser, lhs, 0);

    //NEXT;
}

kuma_node * parse_if_expr(kuma_parser_t *parser)
{
    ACCEPT(TOK_IF);
    kuma_node *cond = parse_expr(parser);
    ACCEPT(TOK_THEN);
    kuma_node *ifblock = parse_block(parser);
    ACCEPT(TOK_ELSE);
    kuma_node *elseblock = parse_block(parser);
    ACCEPT(TOK_END);

    kuma_node *node = (kuma_node *)kuma_if_node_new(LINENO, cond, ifblock, elseblock);

    return node;
}

kuma_node * parse_return(kuma_parser_t *parser)
{
    ACCEPT(TOK_RETURN);

    int expr_count = 0;
    kuma_node *expr_list[KUMA_MAX_RETURNS];

    while(!IS(TOK_NEWLINE))
    {
        if(ACCEPT(TOK_COMMA))
        {
            continue;
        }

        kuma_node *expr = parse_expr(parser);

        if(expr != NULL)
        {
            expr_list[expr_count] = expr;
            expr_count++;
        }
    }

    kuma_node *node = kuma_return_node_new(LINENO, expr_count, expr_list);
    return node;
}

kuma_node * parse_stmnt(kuma_parser_t *parser)
{
    if(IS(TOK_VAR))
        return parse_var_expr(parser);

    if(IS(TOK_LET))
        return parse_let_expr(parser);

    if(IS(TOK_RETURN))
        return parse_return(parser);

    return parse_expr(parser);
}

kuma_node * parse_block(kuma_parser_t *parser)
{
    kuma_block_node *block = kuma_block_node_new(LINENO);

    while (!IS(TOK_END) && !IS(TOK_ELSE))
    {
        kuma_node *node = parse_stmnt(parser);
        if (node)
        {
            klist_add(block->stmts, node);
        }

        ACCEPT(TOK_NEWLINE);
    }

    return (kuma_node *)block;
}

kuma_block_node * kuma_parser_parse(kuma_parser_t *parser)
{
    // Start Lexer
    NEXT;

    kuma_block_node *block = kuma_block_node_new(LINENO);

    while (!IS(TOK_EOF))
    {
        kuma_node *node = parse_stmnt(parser);
        if (node)
        {
            klist_add(block->stmts, node);
        }

        ACCEPT(TOK_NEWLINE);
    }

    return block;
}
