#include <stdint.h>

#ifndef KUMA_OPCODES_H
#define KUMA_OPCODES_H

/*

    Opcode: 8 bits
    'A': 8 bits
    'B': 8 bits
    'C': 8 bits
    'Bx': 16 bits ('B' and 'C')

 */

typedef uint32_t kuma_instruction;

#define SIZE_OP 8
#define SIZE_A 8
#define SIZE_B 8
#define SIZE_C 8
#define SIZE_Bx (SIZE_B + SIZE_C)

#define POS_OP 0
#define POS_A (POS_OP + SIZE_OP)
#define POS_B (POS_A + SIZE_A)
#define POS_C (POS_B + SIZE_B)
#define POS_Bx POS_B

#define GET_OPCODE(i) ((kuma_opcode)(i>>POS_OP)) & (~(0xFFFFFF<<SIZE_OP))
#define GET_ARG_A(i) ((uint32_t)(i>>POS_A)) & (~(0xFFFFFF<<SIZE_A))
#define GET_ARG_B(i) ((uint32_t)(i>>POS_B)) & (~(0xFFFFFF<<SIZE_B))
#define GET_ARG_C(i) ((uint32_t)(i>>POS_C)) & (~(0xFFFFFF<<SIZE_C))
#define GET_ARG_Bx(i) ((uint32_t)(i>>POS_Bx)) & (~(0xFFFFFF<<SIZE_Bx))

#define CREATE_ABC(o,a,b,c)	(((kuma_opcode)o)<<POS_OP) \
			| (((uint8_t)a)<<POS_A) \
			| (((uint8_t)b)<<POS_B) \
			| (((uint8_t)c)<<POS_C)

#define CREATE_ABx(o,a,bx)	(((kuma_opcode)o)<<POS_OP) \
			| (((uint8_t)a)<<POS_A) \
			| (((uint16_t)bx)<<POS_Bx)

typedef enum
{
    OP_LOADCONSTANT,
    OP_ADD,
    OP_HALT

} kuma_opcode;



#endif
