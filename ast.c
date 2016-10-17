#include "ast.h"
#include "string.h"

int indent_level = 0;

#define INDENT \
    for(int i = 0; i < indent_level; i++) { printf("\t"); } \

kuma_ident_node * kuma_ident_node_new(int lineno, char *name)
{
    kuma_ident_node *node = malloc(sizeof(kuma_ident_node));
    node->base.type = IDENTIFIER;
    node->base.lineno = lineno;
    node->name = memcpy(malloc(strlen(name)), name, strlen(name));

    return node;
}

kuma_var_node * kuma_var_node_new(int lineno, char *name, kuma_node *type, kuma_node *expr)
{
    kuma_var_node *node = malloc(sizeof(kuma_var_node));
    node->base.type = VAR_DECL;
    node->base.lineno = lineno;
    node->name = memcpy(malloc(strlen(name)), name, strlen(name));
    node->expr = expr;
    node->type = type;

    return node;
}

kuma_let_node * kuma_let_node_new(int lineno, char *name, kuma_node *type, kuma_node *expr)
{
    kuma_let_node *node = malloc(sizeof(kuma_let_node));
    node->base.type = LET_DECL;
    node->base.lineno = lineno;
    node->name = memcpy(malloc(strlen(name)), name, strlen(name));
    node->expr = expr;
    node->type = type;

    return node;
}

kuma_block_node * kuma_block_node_new(int lineno)
{
    kuma_block_node *node = malloc(sizeof(kuma_block_node));
    node->base.type = BLOCK;
    node->base.lineno = lineno;
    node->stmts = klist_new();

    return node;
}

int dump_var_node(kuma_var_node *node)
{
    INDENT;
    char *name = node->name;
    char *type = ((kuma_ident_node*)node->type)->name;
    printf("VAR_DECL %s %s\n", name, type);
}

int dump_block(kuma_block_node *block)
{
    INDENT;

    printf("BLOCK\n");
    indent_level++;

    for(int i = 0; i < klist_size(block->stmts); i++)
    {
        kuma_node *node = klist_get_at(kuma_node*, block->stmts, i);

        if(node && node->type == VAR_DECL) dump_var_node((kuma_var_node*)node);
    }

    indent_level--;
}

int kuma_ast_dump(kuma_block_node *block)
{
    dump_block(block);
}
