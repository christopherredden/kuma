#include "stdlib.h"
#include "stdio.h"
#include "string.h"

#include "lexer.h"
#include "parser.h"
#include "opcodes.h"
#include "vm.h"
#include "gen.h"

int main(int argc, char* argv[])
{
    FILE* f = fopen(argv[1], "rb");
    if(f == 0)
    {
        exit(1);
    }

    fseek(f, 0, SEEK_END);
    unsigned size = ftell(f) + 1;
    fseek(f, 0, SEEK_SET);
    char *buf = (char*)malloc(sizeof(char) * size);
    fread(buf, sizeof(char), size-1, f);
    buf[size-1] = 0x00;
    fclose(f);

    kuma_lexer_t lex;
    kuma_lexer_init(&lex, buf, argv[1]);
    //kuma_lexer_dump(&lex);

    kuma_parser_t parser;
    kuma_parser_init(&parser, &lex);
    kuma_block_node *program = kuma_parser_parse(&parser);

    kuma_ast_dump(program);

    uint8_t *data = kuma_gen(program);


    /*kuma_instruction i = CREATE_ABC(0xFF, 0xAA, 0xBB, 0xCC);
    kuma_instruction i2 = CREATE_ABx(0xFF, 0xAA, 0xBBCC);
    uint32_t op = GET_OPCODE(i);
    uint32_t a = GET_ARG_A(i);
    uint32_t b = GET_ARG_B(i);
    uint32_t c = GET_ARG_C(i);
    uint32_t bx = GET_ARG_Bx(i2);

    printf("%#010x\n", i);
    printf("%#010x\n", op);
    printf("%#010x\n", a);
    printf("%#010x\n", b);
    printf("%#010x\n", c);
    printf("%#010x\n", bx);*/

    kuma_vm vm;
    kuma_vm_init(&vm, data);
    kuma_vm_execute(&vm);
}
