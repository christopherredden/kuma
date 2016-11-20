#ifndef KUMA_AST_H
#define KUMA_AST_H

#include "list.h"

enum NodeType
{
    NODE_BLOCK = 0,
    NODE_VAR_DECL,
    NODE_LET_DECL,
    NODE_IDENTIFIER,
    NODE_INTEGER,
    NODE_BINOP,
};

typedef struct 
{
    int type;
    int lineno;
} kuma_node;

typedef struct
{
    kuma_node base;
    int value;
} kuma_integer_node;

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

typedef struct
{
    kuma_node base;
    int binop;
    kuma_node *lhs;
    kuma_node *rhs;
} kuma_binop_node;

kuma_binop_node * kuma_binop_node_new(int lineno, int binop, kuma_node *lhs, kuma_node *rhs);

kuma_integer_node * kuma_integer_node_new(int lineno, int value);

kuma_ident_node * kuma_ident_node_new(int lineno, char *name);

kuma_var_node * kuma_var_node_new(int lineno, char *name, kuma_node *type, kuma_node *expr);

kuma_let_node * kuma_let_node_new(int lineno, char *name, kuma_node *type, kuma_node *expr);

kuma_block_node * kuma_block_node_new(int lineno);

int kuma_ast_dump(kuma_block_node *block);

#endif
