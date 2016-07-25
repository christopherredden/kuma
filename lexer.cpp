#include <string>
#include <cstdlib>
#include <cstring>
#include <math.h>
#include <stdio.h>

using namespace std;

#define MAX_IDENT_LEN 2048 * 4

enum TokenType
{
    TOK_EOF = 258,

    TOK_EXTERN,
    TOK_RETURN,
    TOK_BREAK,
    TOK_CONTINUE,
    TOK_CLASS,
    TOK_DEF,
    TOK_END,
    TOK_LET,
    TOK_VAR,
    
    TOK_IDENTIFIER,
    TOK_STRING,
    TOK_NUMBER,
    TOK_INTEGER,

    TOK_EQUAL,    
    TOK_NOT,
    TOK_CEQ,    
    TOK_CNE,
    TOK_CLT,
    TOK_CLE,
    TOK_CGT,
    TOK_CGE,

    TOK_COLON,
    TOK_LPAREN,
    TOK_RPAREN,
    TOK_LBRACE,
    TOK_RBRACE,

    TOK_DOT,
    TOK_COMMA,

    TOK_PLUS,
    TOK_MINUS,
    TOK_MUL,
    TOK_DIV,

    TOK_ILLEGAL,
};

typedef struct
{
    int type;
    const char* name;

    union
    {
        char* string;
        double number;
        int integer;
    } value;

} kuma_token_t;

typedef struct 
{
    const char* filename;
    char* source;
    off_t cursor;
    char last;

    int lineno;
    const char* error;

    kuma_token_t tok;

} kuma_lexer_t;

#define TOKEN(tk) (lex->tok.type = tk), (lex->tok.name = #tk), tk

#define ERROR(msg) (lex->error = msg, TOKEN(TOK_ILLEGAL))

#define NEXT (lex->last = lex->source[lex->cursor++])

#define PREV (lex->source[--lex->cursor] = lex->last)

#define PEEK (lex->source[lex->cursor+1])

#define CURRENT (lex->last)

/*char next()
{
    if(lex.pc == 0)
    {
        lex.pc = lex.buf;
    }
    else if(*lex.pc != EOF)
    {
        lex.pc++;
    }

    return *lex.pc;
}

char prev()
{
    if(lex.pc == 0)
    {
        lex.pc = lex.buf;
    }
    else if(lex.pc != lex.buf)
    {
        lex.pc--;
    }

    return *lex.pc;
}

char peek()
{
    if(lex.pc == 0)
    {
        lex.pc = lex.buf;
    }
    else if(*lex.pc != EOF)
    {
        char* peek = lex.pc + 1;
        return *peek;
    }

    return *lex.pc;
}

char current()
{
    if(lex.pc == 0)
    {
        lex.pc = lex.buf;
    }

    return *lex.pc;
}*/

char *strdup (const char *s) 
{
    char *d = (char*)malloc (strlen (s) + 1);
    if (d == NULL) return NULL;
    strcpy (d,s);
    return d;
}

int scan_string(kuma_lexer_t *lex, int c)
{
    return 0;
}

int scan_ident(kuma_lexer_t *lex, int c)
{
    int len = 0;
    char buf[MAX_IDENT_LEN];

    // TODO: Error check long idents
    buf[len++] = c;
    c = NEXT;
    while(isalnum(c) || c == '_')
    {
        buf[len++] = c;
        c = NEXT;
    }
    PREV;
    buf[len++] = 0;

    // Reserved Keywords
    switch (len-1) 
    {
        case 3:
            if(strcmp("def", buf) == 0) return TOKEN(TOK_DEF);
            if(strcmp("end", buf) == 0) return TOKEN(TOK_END);
            if(strcmp("let", buf) == 0) return TOKEN(TOK_LET);
            if(strcmp("var", buf) == 0) return TOKEN(TOK_VAR);
        case 5:
            if(strcmp("class", buf) == 0) return TOKEN(TOK_CLASS);
        case 6:
            if(strcmp("extern", buf) == 0) return TOKEN(TOK_EXTERN);
            if(strcmp("return", buf) == 0) return TOKEN(TOK_RETURN);
        case 8:
            if(strcmp("continue", buf) == 0) return TOKEN(TOK_CONTINUE);
    }

    // Save Ident String
    lex->tok.value.string = strdup(buf);

    return TOKEN(TOK_IDENTIFIER);
}

int scan_number(kuma_lexer_t *lex, int c)
{
    int v = 0;
    while(isdigit(c))
    {
        v = v * 10 + c - '0';
        c = NEXT;
    }
    PREV;

    if(c == '.')
    {
        int e = 1;
        c = NEXT;
        while(isdigit(c))
        {
            v = v * 10 + c - '0';
            e *= 10;
            c = NEXT;
        }
        PREV;

        lex->tok.value.number = (double)v / (double)e;

        return TOKEN(TOK_NUMBER);
    }

    lex->tok.value.integer = v;
    return TOKEN(TOK_INTEGER);
}

int scan(kuma_lexer_t *lex)
{
scan:
    int c;
    switch(c = NEXT)
    {
        case ' ':
        case '\t': goto scan;
        case ':': return TOKEN(TOK_COLON);
        case '(': return TOKEN(TOK_LPAREN);
        case ')': return TOKEN(TOK_RPAREN);
        case '{': return TOKEN(TOK_LBRACE);
        case '}': return TOKEN(TOK_RBRACE);
        case '.': return TOKEN(TOK_DOT);
        case ',': return TOKEN(TOK_COMMA);
        case '+': return TOKEN(TOK_PLUS);
        case '-': return TOKEN(TOK_MINUS);
        case '*': return TOKEN(TOK_MUL);
        case '/': return TOKEN(TOK_DIV);
        case '=':
                  if(PEEK == '=')
                      return NEXT, TOKEN(TOK_CEQ);
                  return TOKEN(TOK_EQUAL);
        case '!':
                  if(PEEK == '=')
                      return NEXT, TOKEN(TOK_CNE);
                  return TOKEN(TOK_NOT);
        case '<':
                  if(PEEK == '=')
                      return NEXT, TOKEN(TOK_CLE);
                  return TOKEN(TOK_CLT);
        case '>':
                  if(PEEK == '=')
                      return NEXT, TOKEN(TOK_CGE);
                  return TOKEN(TOK_CGT);
        case 0x00: return TOKEN(TOK_EOF);
        case '"':
        case '\'':
                  return scan_string(lex, c);
        default:
                  if (isalpha(c) || c == '_') return scan_ident(lex, c);
                  if (isdigit(c) || c == '.') return scan_number(lex, c);
                  TOKEN(TOK_ILLEGAL);
                  ERROR("Illegal character!");

    }

    return 0;
}

int kuma_lexer_init(kuma_lexer_t *lex, char *source, const char *filename)
{
    lex->error = NULL;
    lex->source = source;
    lex->filename = filename;
    lex->lineno = 1;
    lex->cursor = 0;

    return 0;
}

void lexer(kuma_lexer_t *lex)
{
    while(true)
    {
        int tok = scan(lex);
        switch(tok)
        {
            case TOK_EOF:
                printf("TOK_EOF\n");
                return;
            case TOK_IDENTIFIER:
                printf("TOK_IDENTIFIER '%s'\n", lex->tok.value.string);
                break;
            case TOK_NUMBER:
                printf("TOK_NUMBER '%g'\n", lex->tok.value.number);
                break;
            case TOK_INTEGER:
                printf("TOK_INTEGER '%d'\n", lex->tok.value.integer);
                break;
            default:
                if(tok != 0)
                {
                    printf("%s\n", lex->tok.name);
                }
                break;
        }
    }
}

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
    lexer(&lex);
}
