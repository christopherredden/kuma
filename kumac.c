#include "stdlib.h"
#include "stdio.h"
#include "string.h"

#include "lexer.h"
#include "parser.h"

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
}
