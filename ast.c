#include "ast.h"
#include "string.h"
#include "lexer.h"

int indent_level = 0;

#define INDENT \
    for(int i = 0; i < indent_level; i++) { printf("\t"); } \

kuma_param_node * kuma_param_node_new(int lineno, char *name, char *type)
{
    kuma_param_node *node = malloc(sizeof(kuma_param_node));
    node->base.type = NODE_PARAM;
    node->base.lineno = lineno;
    node->name = memcpy(malloc(strlen(name)), name, strlen(name));
    node->type = memcpy(malloc(strlen(type)), type, strlen(type));

    return node;
}

kuma_return_node * kuma_return_node_new(int lineno, int expr_count, kuma_node **expr_list)
{
    kuma_return_node *node = malloc(sizeof(kuma_return_node));
    node->base.type = NODE_RETURN;
    node->base.lineno = lineno;
    node->expr_count = expr_count;

    for(int i = 0; i < expr_count; i++)
    {
        node->expr_list[i] = expr_list[i];
    }

    return node;
}

kuma_call_node * kuma_call_node_new(int lineno, char *name, int expr_count, kuma_node **expr_list)
{
    kuma_call_node *node = malloc(sizeof(kuma_call_node));
    node->base.type = NODE_CALL;
    node->base.lineno = lineno;
    node->expr_count = expr_count;
    node->name = memcpy(malloc(strlen(name)), name, strlen(name));

    for(int i = 0; i < expr_count; i++)
    {
        node->expr_list[i] = expr_list[i];
    }

    return node;
}

kuma_function_node * kuma_function_node_new(int lineno, char *name, int param_count, kuma_node **param_list, int return_count, kuma_node **return_list, kuma_node *body)
{
    kuma_function_node *node = malloc(sizeof(kuma_function_node));
    node->base.type = NODE_FUNCTION;
    node->base.lineno = lineno;
    node->name = memcpy(malloc(strlen(name)), name, strlen(name));
    node->param_count = param_count;
    node->return_count = return_count;
    node->body = body;

    for(int i = 0; i < param_count; i++)
    {
        node->param_list[i] = param_list[i];
    }

    for(int i = 0; i < return_count; i++)
    {
        node->return_list[i] = return_list[i];
    }

    return node;
}

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
    node->binop = binop;

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

kuma_if_node * kuma_if_node_new(int lineno, kuma_node *cond, kuma_node *ifblock, kuma_node *elseblock)
{
    kuma_if_node *node = malloc(sizeof(kuma_if_node));
    node->base.type = NODE_IF;
    node->base.lineno = lineno;
    node->cond = cond;
    node->ifblock = ifblock;
    node->elseblock = elseblock;

    return node;
}

kuma_assignment_node * kuma_assignment_node_new(int lineno, char *name, kuma_node *expr)
{
    kuma_assignment_node *node = malloc(sizeof(kuma_assignment_node));
    node->base.type = NODE_ASSIGNMENT;
    node->base.lineno = lineno;
    node->name = memcpy(malloc(strlen(name)), name, strlen(name));
    node->expr = expr;

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

    printf("BINOP line:%i op:%i token:%s\n", node->base.lineno, node->binop, kuma_lexer_token_string(node->binop));

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

int dump_if_node(kuma_if_node *node)
{
    INDENT;
    printf("IF_STMNT line:%i\n", node->base.lineno);
    indent_level++;
    dump_node(node->cond);
    dump_node(node->ifblock);
    dump_node(node->elseblock);
    indent_level--;
}

int dump_assignment_node(kuma_assignment_node *node)
{
    INDENT;
    printf("ASSIGNMENT line:%i %s\n", node->base.lineno, node->name);
    indent_level++;
    dump_node(node->expr);
    indent_level--;
}

int dump_return_node(kuma_return_node *node)
{
    INDENT;
    printf("RETURN line:%i\n", node->base.lineno);
    indent_level++;
    for (int i = 0; i < node->expr_count; i++)
    {
        dump_node(node->expr_list[i]);
    }
    indent_level--;
}

int dump_call_node(kuma_call_node *node)
{
    INDENT;
    printf("CALL line:%i %s\n", node->base.lineno, node->name);
    indent_level++;
    for (int i = 0; i < node->expr_count; i++)
    {
        dump_node(node->expr_list[i]);
    }
    indent_level--;
}

int dump_function_node(kuma_function_node *node)
{
    INDENT;
    printf("FUNCTION line:%i %s\n", node->base.lineno, node->name);
    indent_level++;
    INDENT
    printf("PARAMS\n");
    indent_level++;
    for (int i = 0; i < node->param_count; i++)
    {
        dump_node(node->param_list[i]);
    }
    indent_level--;
    INDENT
    printf("RETURNS\n");
    indent_level++;
    for (int i = 0; i < node->return_count; i++)
    {
        dump_node(node->return_list[i]);
    }
    indent_level--;
    INDENT
    printf("BODY\n");
    indent_level++;
    dump_node(node->body);
    indent_level--;
    indent_level--;
}

int dump_param_node(kuma_param_node *node)
{
    INDENT;
    printf("PARAM line:%i %s %s\n", node->base.lineno, node->name, node->type);
}

int dump_node(kuma_node *node)
{
    if(node && node->type == NODE_VAR_DECL) return dump_var_node((kuma_var_node*)node);
    if(node && node->type == NODE_LET_DECL) return dump_let_node((kuma_let_node*)node);
    if(node && node->type == NODE_INTEGER) return dump_integer_node((kuma_integer_node*)node);
    if(node && node->type == NODE_BINOP) return dump_binop_node((kuma_binop_node*)node);
    if(node && node->type == NODE_IDENTIFIER) return dump_ident_node((kuma_ident_node*)node);
    if(node && node->type == NODE_IF) return dump_if_node((kuma_if_node*)node);
    if(node && node->type == NODE_BLOCK) return dump_block((kuma_block_node*)node);
    if(node && node->type == NODE_ASSIGNMENT) return dump_assignment_node((kuma_assignment_node*)node);
    if(node && node->type == NODE_CALL) return dump_call_node((kuma_call_node*)node);
    if(node && node->type == NODE_FUNCTION) return dump_function_node((kuma_function_node*)node);
    if(node && node->type == NODE_PARAM) return dump_param_node((kuma_param_node*)node);
    if(node && node->type == NODE_RETURN) return dump_return_node((kuma_return_node*)node);
}

int dump_block(kuma_block_node *block)
{
    INDENT;

    printf("BLOCK line:%i\n", block->base.lineno);
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
