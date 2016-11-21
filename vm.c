#include <memory.h>

#include "vm.h"

int kuma_vm_init(kuma_vm *vm, uint8_t *data)
{
    uint8_t *dp = data;
    vm->main.nregisters = (uint32_t)*dp;
    dp +=4;

    vm->main.nconstants = (uint32_t)*dp;
    dp +=4;

    vm->main.registers = malloc(sizeof(kuma_value) * vm->main.nregisters);
    memset(vm->main.registers, 0x00, sizeof(kuma_value) * vm->main.nregisters);
    vm->main.constants = (kuma_value *)dp;

    dp += sizeof(kuma_value) * vm->main.nconstants;

    vm->ops = (kuma_instruction *)dp;

    return 0;
}

int kuma_vm_execute(kuma_vm *vm)
{
    printf("\n");
    kuma_opcodes_dump(vm->ops);
    printf("\n");

    kuma_instruction *ip = vm->ops;
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
                vm->main.registers[GET_ARG_A(i)] = vm->main.constants[GET_ARG_Bx(i)];
                break;

            case OP_ADD:
                printf("OP_ADD %i %i %i\n", GET_ARG_A(i), GET_ARG_B(i), GET_ARG_C(i));
                vm->main.registers[GET_ARG_A(i)].int_val = vm->main.registers[GET_ARG_B(i)].int_val + vm->main.registers[GET_ARG_C(i)].int_val;
                break;

            case OP_JMP:
                printf("OP_JMP %i\n", GET_ARG_A(i));
                ip += GET_ARG_A(i);
                break;

            case OP_LOADBOOL:
                printf("OP_LOADBOOL %i %i\n", GET_ARG_A(i), GET_ARG_B(i));
                vm->main.registers[GET_ARG_A(i)].int_val = GET_ARG_B(i);
                break;

            case OP_MOVE:
                printf("OP_MOVE %i %i\n", GET_ARG_A(i), GET_ARG_B(i));
                vm->main.registers[GET_ARG_A(i)] = vm->main.registers[GET_ARG_B(i)];
                break;

            case OP_EQ:
                printf("OP_EQ %i %i %i\n", GET_ARG_A(i), GET_ARG_B(i), GET_ARG_C(i));
                kuma_value a = vm->main.registers[GET_ARG_A(i)];
                kuma_value b = vm->main.registers[GET_ARG_B(i)];
                kuma_value c = vm->main.registers[GET_ARG_C(i)];
                if((b.int_val == c.int_val) == a.int_val)
                {
                    ip++;
                }

                break;

            case OP_HALT:
                printf("OP_HALT\n");
                halted = 1;
                break;
        }
    }

    for(int i = 0; i < vm->main.nregisters; i++)
    {
        printf("Register %i - %i\n", i, vm->main.registers[i].int_val);
    }

    return 0;
}
