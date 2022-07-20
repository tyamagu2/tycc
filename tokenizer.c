#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tycc.h"

static void error_at(char *input, char *loc, char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);

    int pos = loc - input;
    fprintf(stderr, "%s\n", input);
    fprintf(stderr, "%*s", pos, ""); // Print pos spaces.
    fprintf(stderr, "^ ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");

    exit(1);
}

static Token *new_token(TokenKind kind, char *str, int len)
{
    Token *tok = calloc(1, sizeof(Token));
    tok->kind = kind;
    tok->str = str;
    tok->len = len;
    return tok;
}

static int read_punct(char *p)
{
    if (strchr("+-", *p))
    {
        return 1;
    }
    return 0;
}

Token *tokenize(char *p)
{
    char *input = p;
    Token head;
    head.next = NULL;
    Token *cur = &head;

    while (*p)
    {
        if (isspace(*p))
        {
            p++;
            continue;
        }

        if (isdigit(*p))
        {
            cur = cur->next = new_token(TK_NUM, p, 0);
            char *q = p;
            cur->val = strtol(q, &p, 10);
            cur->len = q - p;
            continue;
        }

        int punct_len = read_punct(p);
        if (punct_len > 0)
        {
            cur = cur->next = new_token(TK_PUNCT, p, punct_len);
            p += punct_len;
            continue;
        }

        error_at(input, p, "invalid character");
    }

    cur->next = new_token(TK_EOF, p, 0);
    return head.next;
}