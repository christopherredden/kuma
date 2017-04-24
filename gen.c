#include <memory.h>

#include "gen.h"
#include "list.h"
#include "vm.h"
#include "ast.h"
#include "opcodes.h"
#include "table.h"
#include "lexer.h"

#define MAX_CONSTANTS 256

typedef struct
{
    ktable *idents;
    klist *oplist;

    uint32_t num_constants;
    kuma_value constants[MAX_CONSTANTS];

    uint32_t num_registers;
    uint32_t num_params;
    uint32_t num_returns;
} kuma_gen_func_frame;

typedef struct
{
    // Current Block Context
    kuma_gen_func_frame *ctx;

    // Primary script context
    kuma_gen_func_frame *main;

    klist *functions;
    ktable *kfunctions;

} kuma_gen_t;

kuma_gen_func_frame * kuma_gen_func_context_new()
{
    kuma_gen_func_frame *ctx = malloc(sizeof(kuma_gen_func_frame));

    ctx->oplist = klist_new();
    ctx->num_constants = 0;
    ctx->num_registers = 0;
    ctx->num_params = 0;
    ctx->num_returns = 0;
    ctx->idents = ktable_new();

    return ctx;
}

void kuma_gen_func_context_destroy(kuma_gen_func_frame *ctx)
{
    klist_destroy(ctx->oplist);
    ktable_destroy(ctx->idents);

    free(ctx);
}

int gen_var_node(kuma_gen_t *gen, kuma_var_node *node)
{
    int val_reg = gen_node(gen, node->expr);

    ktable_set(gen->ctx->idents, node->name, val_reg);

    return val_reg;
}

int gen_let_node(kuma_gen_t *gen, kuma_let_node *node)
{
    int val_reg = gen_node(gen, node->expr);

    ktable_set(gen->ctx->idents, node->name, val_reg);

    return val_reg;
}

int gen_integer_node(kuma_gen_t *gen, kuma_integer_node *node)
{
    uint32_t index = gen->ctx->num_constants++;
    uint32_t reg = gen->ctx->num_registers++;

    gen->ctx->constants[index].int_val = node->value;

    int op = CREATE_ABx(OP_LOADCONSTANT, (uint8_t)reg, (uint16_t)index);
    klist_add(gen->ctx->oplist, op);

    return reg;
}

int gen_binop_node(kuma_gen_t *gen, kuma_binop_node *node)
{
    uint32_t lhs = gen_node(gen, node->lhs);
    uint32_t rhs = gen_node(gen, node->rhs);
    uint32_t reg = gen->ctx->num_registers++;

    int binop = 0;
    switch(node->binop)
    {
        case TOK_PLUS:
            binop = OP_ADD;
            break;
        case TOK_MINUS:
            binop = OP_SUB;
            break;
        case TOK_MUL:
            binop = OP_MUL;
            break;
        case TOK_DIV:
            binop = OP_DIV;
            break;
        case TOK_CEQ:
            binop = OP_EQ;
            break;
        case TOK_CGE:
            binop = OP_GEQ;
            break;
        case TOK_CGT:
            binop = OP_GT;
            break;
        case TOK_CLE:
            binop = OP_LEQ;
            break;
        case TOK_CLT:
            binop = OP_LT;
            break;
    }

    if(binop == OP_EQ || binop == OP_GEQ || binop == OP_GT || binop == OP_LEQ || binop == OP_LT)
    {
        klist_add(gen->ctx->oplist, CREATE_ABx(OP_LOADBOOL, reg, 1));
        klist_add(gen->ctx->oplist, CREATE_ABC(binop, reg, lhs, rhs));
        klist_add(gen->ctx->oplist, CREATE_ABx(OP_LOADBOOL, reg, 0));
    }
    else
    {
        klist_add(gen->ctx->oplist, CREATE_ABC(binop, reg, lhs, rhs));
    }

    return reg;
}

int gen_ident_node(kuma_gen_t *gen, kuma_ident_node *node)
{
    int reg = ktable_get(gen->ctx->idents, node->name);
    return reg;
}

int gen_if_node(kuma_gen_t *gen, kuma_if_node *node)
{
    int cond = gen_node(gen, node->cond);

    uint32_t boolreg = gen->ctx->num_registers++;
    uint32_t resultreg = gen->ctx->num_registers++;

    klist_add(gen->ctx->oplist, CREATE_ABx(OP_LOADBOOL, boolreg, 1));
    klist_add(gen->ctx->oplist, CREATE_ABC(OP_EQ, boolreg, cond, boolreg));
    size_t prevsize = klist_size(gen->ctx->oplist);

    int ifreg = gen_node(gen, node->ifblock);
    klist_add(gen->ctx->oplist, CREATE_ABC(OP_MOVE, resultreg, ifreg, 0)); // Store If result
    int ifjmp = (int)(klist_size(gen->ctx->oplist) - prevsize) + 1; // Jump over IF
    klist_add_at(gen->ctx->oplist, CREATE_ABC(OP_JMP, ifjmp, 0, 0), prevsize); // Insert jump to Else
    prevsize = klist_size(gen->ctx->oplist);

    int elsereg = gen_node(gen, node->elseblock);
    klist_add(gen->ctx->oplist, CREATE_ABC(OP_MOVE, resultreg, elsereg, 0)); // Store Else result
    int elsejmp = (int)(klist_size(gen->ctx->oplist) - prevsize);
    klist_add_at(gen->ctx->oplist, CREATE_ABC(OP_JMP, elsejmp, 0, 0), prevsize); // Insert jump to Else

    return resultreg;
}

int gen_assignment_node(kuma_gen_t *gen, kuma_assignment_node *node)
{
    int ident = (int)ktable_get(gen->ctx->idents, node->name);
    int r = gen_node(gen, node->expr);

    klist_add(gen->ctx->oplist, CREATE_ABC(OP_MOVE, ident, r, 0));

    return ident;
}

int gen_block_node(kuma_gen_t *gen, kuma_block_node *node)
{
    int r = 0;
    for(int i = 0; i < klist_size(node->stmts); i++)
    {
        r = gen_node(gen, klist_get_at(kuma_node*, node->stmts, i));
    }

    return r;
}

int gen_call_node(kuma_gen_t *gen, kuma_call_node *node)
{
    return NULL;
}

int gen_node_function(kuma_gen_t *gen, kuma_function_node *node)
{
    // Push new function context
    kuma_gen_func_frame *ctx = kuma_gen_func_context_new();
    klist_add(gen->functions, ctx);
    ktable_set(gen->kfunctions, node->name, ctx);
    gen->ctx = ctx;
    ctx->num_params = node->param_count;
    ctx->num_returns = node->return_count;

    int r = 0;
    for(int i = 0; i < node->param_count; i++)
    {
        r = gen_node(gen, node->param_list[i]);
    }

    int body = gen_node(gen, node->body);

    //node->body

    // Pop Function Context
    gen->ctx = gen->main;

    return body;
}

int gen_param_node(kuma_gen_t *gen, kuma_param_node *node)
{
    uint32_t reg = gen->ctx->num_registers++;
    ktable_set(gen->ctx->idents, node->name, reg);

    return reg;
}

int gen_return_node(kuma_gen_t *gen, kuma_return_node *node)
{
    return 0;
}

int gen_node(kuma_gen_t *gen, kuma_node *node)
{
    if(node && node->type == NODE_VAR_DECL) return gen_var_node(gen, (kuma_var_node*)node);
    if(node && node->type == NODE_PARAM) return gen_param_node(gen, (kuma_param_node*)node);
    if(node && node->type == NODE_RETURN) return gen_return_node(gen, (kuma_return_node*)node);
    if(node && node->type == NODE_LET_DECL) return gen_let_node(gen, (kuma_let_node*)node);
    if(node && node->type == NODE_INTEGER) return gen_integer_node(gen, (kuma_integer_node*)node);
    if(node && node->type == NODE_BINOP) return gen_binop_node(gen, (kuma_binop_node*)node);
    if(node && node->type == NODE_IDENTIFIER) return gen_ident_node(gen, (kuma_ident_node*)node);
    if(node && node->type == NODE_IF) return gen_if_node(gen, (kuma_if_node*)node);
    if(node && node->type == NODE_ASSIGNMENT) return gen_assignment_node(gen, (kuma_assignment_node*)node);
    if(node && node->type == NODE_BLOCK) return gen_block_node(gen, (kuma_block_node*)node);
    if(node && node->type == NODE_FUNCTION) return gen_node_function(gen, (kuma_function_node*)node);
    if(node && node->type == NODE_CALL) return gen_call_node(gen, (kuma_call_node*)node);

    return 0;
}

uint8_t * kuma_gen(kuma_block_node *block)
{
    kuma_gen_t gen;
    gen.functions = klist_new();
    gen.kfunctions = ktable_new();
    gen.main = kuma_gen_func_context_new();

    // Add Main Function
    klist_add(gen.functions, gen.main);

    // Generate Code
    for(int i = 0; i < klist_size(block->stmts); i++)
    {
        kuma_node *node = klist_get_at(kuma_node*, block->stmts, i);
        gen_node(&gen, node);
    }


    // Data Size
    size_t sdata = 0;

    // Count function data size
    for(uint32_t i = 0; i < klist_size(gen.functions); i++)
    {
        // Get Function
        kuma_gen_func_frame *ctx = klist_get_at(kuma_gen_func_frame*, gen.functions, i);

        // Func Header
        // nparams : uint32
        // nreturns : uint32
        // nregisters : uint32
        // ninstructions : uint32
        size_t sheader = sizeof(uint32_t) * 4;
        size_t sconstants = sizeof(uint32_t) + (sizeof(kuma_value) * ctx->num_constants);
        size_t sops = sizeof(kuma_instruction) * klist_size(ctx->oplist);

        sdata += (sheader + sconstants + sops);
    }

    // Allocate and get data pointer
    uint8_t *data = malloc(sdata);
    uint8_t *dp = data;

    // Write Each Function
    for(uint32_t i = 0; i < klist_size(gen.functions); i++)
    {
        kuma_gen_func_frame *ctx = klist_get_at(kuma_gen_func_frame*, gen.functions, i);
        uint32_t num_ops = klist_size(ctx->oplist);

        memcpy(dp, &ctx->num_params, sizeof(uint32_t));
        dp += 4;
        memcpy(dp, &ctx->num_returns, sizeof(uint32_t));
        dp += 4;
        memcpy(dp, &ctx->num_registers, sizeof(uint32_t));
        dp += 4;
        memcpy(dp, &num_ops, sizeof(uint32_t));
        dp += 4;

        kuma_instruction *ops = dp;
        for(uint32_t i = 0; i < num_ops; i++)
        {
            ops[i] = klist_get_at(kuma_instruction, ctx->oplist, i);
            dp += sizeof(kuma_instruction);
        }

        memcpy(dp, &ctx->num_constants, sizeof(uint32_t));
        dp += 4;
        memcpy(dp, &ctx->constants, sizeof(kuma_value) * ctx->num_constants);
        dp += sizeof(kuma_value) * ctx->num_constants;
    }

    // Free
    for(uint32_t i = 0; i < klist_size(gen.functions); i++)
    {
        kuma_gen_func_frame *ctx = klist_get_at(kuma_gen_func_frame*, gen.functions, i);

        kuma_gen_func_context_destroy(ctx);
    }

    klist_destroy(gen.functions);
    ktable_destroy(gen.kfunctions);

    return data;
}
