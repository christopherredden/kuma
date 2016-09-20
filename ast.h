#ifndef KUMA_AST_H
#define KUMA_AST_H

enum node_t
{
    N_BLOCK = 0,
    N_VAR_DECL,
};

typedef struct 
{
    node_t type;
    int lineno;
} kuma_node_t;

typedef struct 
{
    kuma_node_t base;
    luna_vec_t *stmts;
} kuma_block_node_t;

typedef struct 
{
    kuma_node_t base;
    char *name;
} kuma_var_node_t;

kuma_var_node_t * kuma_ast_var_node_new();

#endif
