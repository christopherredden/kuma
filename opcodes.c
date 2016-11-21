#include <stdio.h>

#include "opcodes.h"

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
            case OP_LOADCONSTANT:
                printf("OP_LOADCONSTANT %i %i\n", GET_ARG_A(i), GET_ARG_Bx(i));
                break;

            case OP_ADD:
                printf("OP_ADD %i %i %i\n", GET_ARG_A(i), GET_ARG_B(i), GET_ARG_C(i));
                break;

            case OP_JMP:
                printf("OP_JMP %i\n", GET_ARG_A(i));
                break;

            case OP_LOADBOOL:
                printf("OP_LOADBOOL %i %i\n", GET_ARG_A(i), GET_ARG_B(i));
                break;

            case OP_MOVE:
                printf("OP_MOVE %i %i\n", GET_ARG_A(i), GET_ARG_B(i));
                break;

            case OP_EQ:
                printf("OP_EQ %i %i %i\n", GET_ARG_A(i), GET_ARG_B(i), GET_ARG_C(i));
                break;

            case OP_HALT:
                printf("OP_HALT\n");
                halted = 1;
                break;
        }
    }
}
