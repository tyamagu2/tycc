#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tycc.h"

bool equal(Token *tok, char *op)
{
    return tok->len == strlen(op) && memcmp(tok->str, op, tok->len) == 0;
}

static Token *new_token(TokenKind kind, char *str, int len)
{
    Token *tok = calloc(1, sizeof(Token));
    tok->kind = kind;
    tok->str = str;
    tok->len = len;
    return tok;
}

static bool starts_with(const char *p, const char *prefix)
{
    return memcmp(p, prefix, strlen(prefix)) == 0;
}

static bool is_keyword(Token *tok)
{
    static char *kw[] = {"return", "if", "else", "for", "while"};
    for (int i = 0; i < sizeof(kw) / sizeof(*kw); i++)
    {
        if (equal(tok, kw[i]))
        {
            return true;
        }
    }
    return false;
}

static void convert_to_keywords(Token *tok)
{
    while (tok)
    {
        if (tok->kind == TK_IDENT && is_keyword(tok))
        {
            tok->kind = TK_KEYWORD;
        }
        tok = tok->next;
    }
}

// is_punct returns the length of the punctuator at p.
// It returns 0 if p does not point a valid punctuator.
static int is_punct(char *p)
{
    if (starts_with(p, "==") || starts_with(p, "!=") || starts_with(p, "<=") || starts_with(p, ">="))
    {
        return 2;
    }
    if (strchr("+-*/();=<>", *p))
    {
        return 1;
    }
    return 0;
}

// is_ident1 returns true if c is valid as the first leter of an identifier.
static bool is_ident1(char c)
{
    return ('a' <= c && c <= 'z') ||
           ('A' <= c && c <= 'Z') ||
           c == '_';
}

// is_ident2 returns true if c is valid as a non-first letter of an identifier.
static bool is_ident2(char c)
{
    return is_ident1(c) || ('0' <= c && c <= '9');
}

// is_ident returns the length of the identifier at p.
// It returns 0 if p does not point to a valid identifier.
static int is_ident(char *p)
{
    char *from = p;
    if (!is_ident1(*p))
    {
        return 0;
    }
    p++;
    while (is_ident2(*p))
    {
        p++;
    }
    return p - from;
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

        int ident_len = is_ident(p);
        if (ident_len > 0)
        {
            cur = cur->next = new_token(TK_IDENT, p, ident_len);
            p += ident_len;
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

        int punct_len = is_punct(p);
        if (punct_len > 0)
        {
            cur = cur->next = new_token(TK_PUNCT, p, punct_len);
            p += punct_len;
            continue;
        }

        error_at(input, p, "invalid token");
    }

    cur->next = new_token(TK_EOF, p, 0);

    convert_to_keywords(head.next);

    return head.next;
}
