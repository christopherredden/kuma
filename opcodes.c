#include <stdio.h>

#include "opcodes.h"

#define DEBUG_OP(OP) \
    case OP: \
        printf("%s\n", #OP); \
        break;

int kuma_opcodes_dump(kuma_instruction *ops)
{
    kuma_instruction *ip = ops;
    kuma_instruction i;

    int halted = 0;
    while(!halted)
    {
        i = *ip++;
        kuma_opcode op = GET_OPCODE(i);

        switch(op)
        {
            DEBUG_OP(OP_LOADBOOL)
            DEBUG_OP(OP_LOADCONSTANT)
            DEBUG_OP(OP_MOVE)
            DEBUG_OP(OP_ADD)
            DEBUG_OP(OP_SUB)
            DEBUG_OP(OP_MUL)
            DEBUG_OP(OP_DIV)
            DEBUG_OP(OP_JMP)
            DEBUG_OP(OP_EQ)
            DEBUG_OP(OP_LT)
            DEBUG_OP(OP_LEQ)
            DEBUG_OP(OP_GT)
            DEBUG_OP(OP_GEQ)
            DEBUG_OP(OP_CALL)
            DEBUG_OP(OP_NOP)
            DEBUG_OP(OP_HALT)
            DEBUG_OP(OP_EOF)
        }
    }
}
