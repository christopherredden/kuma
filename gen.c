#include <memory.h>

#include "gen.h"
#include "list.h"
#include "vm.h"
#include "ast.h"
#include "opcodes.h"
#include "table.h"

#define MAX_CONSTANTS 32749

typedef struct
{
    ktable *idents;
    klist *oplist;

    uint32_t num_constants;
    kuma_value constants[MAX_CONSTANTS];

    uint32_t num_registers;

} kuma_gen_context;

int gen_var_node(kuma_gen_context *ctx, kuma_var_node *node)
{
    int val_reg = gen_node(ctx, node->expr);

    ktable_set(ctx->idents, node->name, val_reg);

    return val_reg;
}

int gen_let_node(kuma_gen_context *ctx, kuma_let_node *node)
{
    int val_reg = gen_node(ctx, node->expr);

    ktable_set(ctx->idents, node->name, val_reg);

    return val_reg;
}

int gen_integer_node(kuma_gen_context *ctx, kuma_integer_node *node)
{
    uint32_t index = ctx->num_constants++;
    uint32_t reg = ctx->num_registers++;

    ctx->constants[index].int_val = node->value;

    int op = CREATE_ABx(OP_LOADCONSTANT, (uint8_t)reg, (uint16_t)index);
    klist_add(ctx->oplist, op);

    return reg;
}

int gen_binop_node(kuma_gen_context *ctx, kuma_binop_node *node)
{
    uint32_t lhs = gen_node(ctx, node->lhs);
    uint32_t rhs = gen_node(ctx, node->rhs);
    uint32_t reg = ctx->num_registers++;

    int op = CREATE_ABC(OP_ADD, reg, lhs, rhs);
    klist_add(ctx->oplist, op);

    return reg;
}

int gen_ident_node(kuma_gen_context *ctx, kuma_ident_node *node)
{
    int reg = ktable_get(ctx->idents, node->name);
    return reg;
}

int gen_node(kuma_gen_context *ctx, kuma_node *node)
{
    if(node && node->type == NODE_VAR_DECL) return gen_var_node(ctx, (kuma_var_node*)node);
    if(node && node->type == NODE_LET_DECL) return gen_let_node(ctx, (kuma_let_node*)node);
    if(node && node->type == NODE_INTEGER) return gen_integer_node(ctx, (kuma_integer_node*)node);
    if(node && node->type == NODE_BINOP) return gen_binop_node(ctx, (kuma_binop_node*)node);
    if(node && node->type == NODE_IDENTIFIER) return gen_ident_node(ctx, (kuma_ident_node*)node);

    return 0;
}

uint8_t * kuma_gen(kuma_block_node *block)
{
    kuma_gen_context ctx;
    ctx.oplist = klist_new();
    ctx.num_constants = 0;
    ctx.num_registers = 0;
    ctx.idents = ktable_new();

    for(int i = 0; i < klist_size(block->stmts); i++)
    {
        kuma_node *node = klist_get_at(kuma_node*, block->stmts, i);
        gen_node(&ctx, node);
    }

    kuma_instruction haltop = CREATE_ABx(OP_HALT, 0x25, 0x42);
    klist_add(ctx.oplist, haltop);
    uint32_t numops = klist_size(ctx.oplist);

    size_t sregisters = sizeof(uint32_t);
    size_t sconstants = sizeof(uint32_t) + (sizeof(kuma_value) * ctx.num_constants);
    size_t sops = sizeof(kuma_instruction) * numops;
    size_t sdata = sregisters + sconstants + sops;

    uint8_t *data = malloc(sdata);
    uint8_t *dp = data;

    memcpy(dp, &ctx.num_registers, sizeof(uint32_t));
    dp += 4;
    memcpy(dp, &ctx.num_constants, sizeof(uint32_t));
    dp += 4;
    memcpy(dp, &ctx.constants, sizeof(kuma_value) * ctx.num_constants);
    dp += sizeof(kuma_value) * ctx.num_constants;

    kuma_instruction *ops = dp;
    for(uint32_t i = 0; i < numops; i++)
    {
        ops[i] = klist_get_at(kuma_instruction, ctx.oplist, i);
    }

    klist_destroy(ctx.oplist);
    ktable_destroy(ctx.idents);

    return data;
}
