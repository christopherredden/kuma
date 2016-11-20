#include "ast.h"
#include "string.h"

int indent_level = 0;

#define INDENT \
    for(int i = 0; i < indent_level; i++) { printf("\t"); } \

kuma_integer_node * kuma_integer_node_new(int lineno, int value)
{
    kuma_integer_node *node = malloc(sizeof(kuma_integer_node));
    node->base.type = NODE_INTEGER;
    node->base.lineno = lineno;
    node->value = value;

    return node;
}

kuma_ident_node * kuma_ident_node_new(int lineno, char *name)
{
    kuma_ident_node *node = malloc(sizeof(kuma_ident_node));
    node->base.type = NODE_IDENTIFIER;
    node->base.lineno = lineno;
    node->name = memcpy(malloc(strlen(name)), name, strlen(name));

    return node;
}

kuma_var_node * kuma_var_node_new(int lineno, char *name, kuma_node *type, kuma_node *expr)
{
    kuma_var_node *node = malloc(sizeof(kuma_var_node));
    node->base.type = NODE_VAR_DECL;
    node->base.lineno = lineno;
    node->name = memcpy(malloc(strlen(name)), name, strlen(name));
    node->expr = expr;
    node->type = type;

    return node;
}

kuma_let_node * kuma_let_node_new(int lineno, char *name, kuma_node *type, kuma_node *expr)
{
    kuma_let_node *node = malloc(sizeof(kuma_let_node));
    node->base.type = NODE_LET_DECL;
    node->base.lineno = lineno;
    node->name = memcpy(malloc(strlen(name)), name, strlen(name));
    node->expr = expr;
    node->type = type;

    return node;
}

kuma_binop_node * kuma_binop_node_new(int lineno, int binop, kuma_node *lhs, kuma_node *rhs)
{
    kuma_binop_node *node = malloc(sizeof(kuma_binop_node));
    node->base.type = NODE_BINOP;
    node->base.lineno = lineno;
    node->lhs = lhs;
    node->rhs = rhs;

    return node;
}

kuma_block_node * kuma_block_node_new(int lineno)
{
    kuma_block_node *node = malloc(sizeof(kuma_block_node));
    node->base.type = NODE_BLOCK;
    node->base.lineno = lineno;
    node->stmts = klist_new();

    return node;
}

int dump_var_node(kuma_var_node *node)
{
    INDENT;
    char *name = node->name;
    char *type = ((kuma_ident_node*)node->type)->name;
    printf("VAR_DECL line:%i %s %s\n", node->base.lineno, name, type);

    indent_level++;
    dump_node(node->expr);
    indent_level--;
}

int dump_let_node(kuma_let_node *node)
{
    INDENT;
    char *name = node->name;
    char *type = ((kuma_ident_node*)node->type)->name;
    printf("LET_DECL line:%i %s %s\n", node->base.lineno, name, type);

    indent_level++;
    dump_node(node->expr);
    indent_level--;
}

int dump_integer_node(kuma_integer_node *node)
{
    INDENT;
    printf("INTEGER_LITERAL line:%i %i\n", node->base.lineno, node->value);
}

int dump_binop_node(kuma_binop_node *node)
{
    INDENT;
    printf("BINOP line:%i\n", node->base.lineno);

    indent_level++;
    dump_node(node->lhs);
    dump_node(node->rhs);
    indent_level--;
}

int dump_ident_node(kuma_ident_node *node)
{
    INDENT;
    printf("IDENTIFIER line:%i %s\n", node->base.lineno, node->name);
}

int dump_node(kuma_node *node)
{
    if(node && node->type == NODE_VAR_DECL) return dump_var_node((kuma_var_node*)node);
    if(node && node->type == NODE_LET_DECL) return dump_let_node((kuma_let_node*)node);
    if(node && node->type == NODE_INTEGER) return dump_integer_node((kuma_integer_node*)node);
    if(node && node->type == NODE_BINOP) return dump_binop_node((kuma_binop_node*)node);
    if(node && node->type == NODE_IDENTIFIER) return dump_ident_node((kuma_ident_node*)node);
}

int dump_block(kuma_block_node *block)
{
    INDENT;

    printf("BLOCK\n");
    indent_level++;

    for(int i = 0; i < klist_size(block->stmts); i++)
    {
        kuma_node *node = klist_get_at(kuma_node*, block->stmts, i);
        dump_node(node);
    }

    indent_level--;
}

int kuma_ast_dump(kuma_block_node *block)
{
    indent_level = 0;
    dump_block(block);
}
