#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tycc.h"

// For error reporting.
static Token *_start;

bool equal(Token *tok, char *op)
{
    return tok->len == strlen(op) && memcmp(tok->str, op, tok->len) == 0;
}

Token *consume(Token *tok, Token **end, char *op)
{
    if (!equal(tok, op))
    {
        return NULL;
    }

    *end = tok->next;
    return tok;
}

static void error_token(Token *tok, char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    verror_at(_start->str, tok->str, fmt, ap);
}

void expect(Token *tok, Token **end, char *op)
{
    if (!consume(tok, end, op))
    {
        error_token(tok, "expected %s", op);
    }
}

Node *new_node(NodeKind kind, Token *tok)
{
    Node *node = calloc(1, sizeof(Node));
    node->kind = kind;
    node->token = tok;
    return node;
}

Node *new_binary(NodeKind kind, Token *tok, Node *lhs, Node *rhs)
{
    Node *node = new_node(kind, tok);
    node->lhs = lhs;
    node->rhs = rhs;
    return node;
}

Node *new_num(Token *tok)
{
    Node *node = new_node(NK_NUM, tok);
    node->val = tok->val;
    return node;
}

Node *expr(Token *tok, Token **end);
Node *add(Token *tok, Token **end);
Node *mul(Token *tok, Token **end);
Node *primary(Token *tok, Token **end);

// expr
//  = add
Node *expr(Token *tok, Token **end)
{
    Node *node = add(tok, end);
    return node;
}

// add
//  = mul ("+" mul | "-" mul)*
Node *add(Token *tok, Token **end)
{
    Node *node = mul(tok, &tok);

    for (;;)
    {
        if (consume(tok, &tok, "+"))
        {
            node = new_binary(NK_ADD, tok, node, mul(tok, &tok));
            continue;
        }
        if (consume(tok, &tok, "-"))
        {
            node = new_binary(NK_SUB, tok, node, mul(tok, &tok));
            continue;
        }
        break;
    }

    *end = tok;
    return node;
}

// mul
//  = primary ("*" primary | "/" primary)*
Node *mul(Token *tok, Token **end)
{
    Node *node = primary(tok, &tok);

    for (;;)
    {
        if (consume(tok, &tok, "*"))
        {
            node = new_binary(NK_MUL, tok, node, primary(tok, &tok));
            continue;
        }
        if (consume(tok, &tok, "/"))
        {
            node = new_binary(NK_DIV, tok, node, primary(tok, &tok));
            continue;
        }
        break;
    }

    *end = tok;
    return node;
}

// primary
//  = num
//  | "(" expr ")"
Node *primary(Token *tok, Token **end)
{
    if (tok->kind == TK_NUM)
    {
        Node *node = new_num(tok);
        *end = tok->next;
        return node;
    }

    expect(tok, &tok, "(");
    Node *node = expr(tok, &tok);
    expect(tok, &tok, ")");
    *end = tok;
    return node;
}

Node *parse(Token *tok)
{
    _start = tok;
    return expr(tok, &tok);
}
