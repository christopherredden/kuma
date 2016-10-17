#ifndef KUMA_AST_H
#define KUMA_AST_H

#include "list.h"

enum NodeType
{
    BLOCK = 0,
    VAR_DECL,
    LET_DECL,
    IDENTIFIER,
};

typedef struct 
{
    int type;
    int lineno;
} kuma_node;

typedef struct
{
    kuma_node base;
    char *name;
} kuma_ident_node;

typedef struct 
{
    kuma_node base;
    klist *stmts;
} kuma_block_node;

typedef struct 
{
    kuma_node base;
    char *name;
    kuma_node *type;
    kuma_node *expr;
} kuma_var_node;

typedef struct
{
    kuma_node base;
    char *name;
    kuma_node *type;
    kuma_node *expr;
} kuma_let_node;

kuma_ident_node * kuma_ident_node_new(int lineno, char *name);

kuma_var_node * kuma_var_node_new(int lineno, char *name, kuma_node *type, kuma_node *expr);

kuma_let_node * kuma_let_node_new(int lineno, char *name, kuma_node *type, kuma_node *expr);

kuma_block_node * kuma_block_node_new(int lineno);

int kuma_ast_dump(kuma_block_node *block);

#endif
