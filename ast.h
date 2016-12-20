#ifndef KUMA_AST_H
#define KUMA_AST_H

#include "list.h"

#define KUMA_MAX_PARAMS 256
#define KUMA_MAX_RETURNS 256

enum NodeType
{
    NODE_BLOCK = 0,
    NODE_VAR_DECL,
    NODE_LET_DECL,
    NODE_IDENTIFIER,
    NODE_INTEGER,
    NODE_BINOP,
    NODE_IF,
    NODE_ASSIGNMENT,
    NODE_CALL,
    NODE_FUNCTION,
    NODE_PARAM,
    NODE_RETURN,
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

typedef struct
{
    kuma_node base;

    kuma_node *cond;
    kuma_node *ifblock;
    kuma_node *elseblock;
} kuma_if_node;

typedef struct
{
    kuma_node base;

    char *name;
    kuma_node *expr;
} kuma_assignment_node;

typedef struct
{
    kuma_node base;

    char *name;
    int expr_count;
    kuma_node *expr_list[KUMA_MAX_PARAMS];
} kuma_call_node;

typedef struct
{
    kuma_node base;

    char *name;
    int param_count;
    kuma_node *param_list[KUMA_MAX_PARAMS];
    int return_count;
    kuma_node *return_list[KUMA_MAX_RETURNS];
    kuma_node *body;
} kuma_function_node;

typedef struct
{
    kuma_node base;

    char *name;
    char *type;
} kuma_param_node;

typedef struct
{
    kuma_node base;

    int expr_count;
    kuma_node *expr_list[KUMA_MAX_RETURNS];
} kuma_return_node;

kuma_return_node * kuma_return_node_new(int lineno, int expr_count, kuma_node **expr_list);

kuma_param_node * kuma_param_node_new(int lineno, char *name, char *type);

kuma_function_node * kuma_function_node_new(int lineno, char *name, int param_count, kuma_node **param_list, int return_count, kuma_node **return_list, kuma_node *body);

kuma_call_node * kuma_call_node_new(int lineno, char *name, int expr_count, kuma_node **expr_list);

kuma_assignment_node * kuma_assignment_node_new(int lineno, char *name, kuma_node *expr);

kuma_if_node * kuma_if_node_new(int lineno, kuma_node *cond, kuma_node *ifblock, kuma_node *elseblock);

kuma_binop_node * kuma_binop_node_new(int lineno, int binop, kuma_node *lhs, kuma_node *rhs);

kuma_integer_node * kuma_integer_node_new(int lineno, int value);

kuma_ident_node * kuma_ident_node_new(int lineno, char *name);

kuma_var_node * kuma_var_node_new(int lineno, char *name, kuma_node *type, kuma_node *expr);

kuma_let_node * kuma_let_node_new(int lineno, char *name, kuma_node *type, kuma_node *expr);

kuma_block_node * kuma_block_node_new(int lineno);

int kuma_ast_dump(kuma_block_node *block);

#endif
