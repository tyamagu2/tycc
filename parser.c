#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "tycc.h"

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

Node *add(Token *tok, Token **end);
Node *primary(Token *tok, Token **end);

// add
//  = primary ("+" primary | "-" primary)*
Node *add(Token *tok, Token **end)
{
    Node *node = primary(tok, &tok);

    for (;;)
    {
        if (consume(tok, &tok, "+"))
        {
            node = new_binary(NK_ADD, tok, node, primary(tok, &tok));
            continue;
        }
        if (consume(tok, &tok, "-"))
        {
            node = new_binary(NK_SUB, tok, node, primary(tok, &tok));
            continue;
        }
        break;
    }

    *end = tok;
    return node;
}

// primary
//  = number
//  | add
Node *primary(Token *tok, Token **end)
{
    if (tok->kind == TK_NUM)
    {
        Node *node = new_num(tok);
        *end = tok->next;
        return node;
    }

    return add(tok, &tok);
}

Node *parse(Token *tok)
{
    return add(tok, &tok);
}
