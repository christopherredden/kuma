#ifndef KUMA_VM_H
#define KUMA_VM_H

#include "lexer.h"
#include "ast.h"
#include "parser.h"
#include "opcodes.h"

typedef union
{
    int32_t int_val;
} kuma_value;

typedef struct
{
    kuma_value *registers;
    uint32_t nregisters;

    kuma_value *constants;
    uint32_t nconstants;
} kuma_frame;

typedef struct
{
    kuma_instruction *ops;

    kuma_frame main;
} kuma_vm;

int kuma_vm_init(kuma_vm *vm, uint8_t *data);

int kuma_vm_execute(kuma_vm *vm);

#endif //KUMA_VM_H
