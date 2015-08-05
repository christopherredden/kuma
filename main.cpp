/*
 * main.c file
 */
#include <string>
#include <iostream>
#include <fstream>

#include "nodes.h"
#include "kuma_parser.h"
#include "kuma_lexer.h"

#include <stdio.h>

int yyparse(NBlock **expression, yyscan_t scanner);

NBlock *getAST(const char *expr)
{
    NBlock *programBlock;
    yyscan_t scanner;
    YY_BUFFER_STATE state;

    if (yylex_init(&scanner)) {
        // couldn't initialize
        return NULL;
    }

    state = yy_scan_string(expr, scanner);

    if (yyparse(&programBlock, scanner)) {
        // error parsing
        return NULL;
    }

    yy_delete_buffer(state, scanner);

    yylex_destroy(scanner);

    return programBlock;
}

/*int evaluate(SExpression *e)
{
    switch (e->type) {
        case eVALUE:
            return e->value;
        case eMULTIPLY:
            return evaluate(e->left) * evaluate(e->right);
        case ePLUS:
            return evaluate(e->left) + evaluate(e->right);
        default:
            // shouldn't be here
            return 0;
    }
}*/

int main(void)
{
    NBlock *programBlock = NULL;
    /*char test[]="extern void printi(int val)\n"
            "\n"
            "int foo = 5\n"
            "int do_math(int a) {\n"
            "  int x = a * 5\n"
            "  return x + 3\n"
            "}\n"
            "\n"
            "echo(do_math(11))\n"
            "echo(do_math(12))\n"
            "printi(10)";
    int result = 0;*/

    std::ifstream t("test.ks");
    std::string str;

    t.seekg(0, std::ios::end);
    str.reserve(t.tellg());
    t.seekg(0, std::ios::beg);

    str.assign((std::istreambuf_iterator<char>(t)),
               std::istreambuf_iterator<char>());

    programBlock = getAST(str.c_str());

    std::cout << programBlock << std::endl;

    CodeGenContext context;
    context.generateCode(*programBlock);
    context.runCode();


    printf("Completed.\n");
//    printf("%s\n", context.output.str().c_str());

    //result = evaluate(e);

    //printf("Result of '%s' is %d\n", test, result);

    //deleteExpression(e);

    return 0;
}