#include <stdio.h>

#include "tycc.h"

static void dump_token(Token *tok)
{
    switch (tok->kind)
    {
    case TK_PUNCT:
        printf("PUNCT");
        break;
    case TK_NUM:
        printf("NUM");
        break;
    case TK_EOF:
        puts("EOF");
        return;
    default:
        printf("INVALID_TOKEN_KIND=%d", tok->kind);
    }

    if (tok->len > 0)
    {
        printf("(%.*s)", tok->len, tok->str);
    }
    puts("");
}

void dump_tokens(Token *tok)
{
    puts("----TOKENS----");
    while (tok)
    {
        dump_token(tok);
        tok = tok->next;
    }
    puts("--------------");
}

void dump_node(Node *node, int depth, char *prefix)
{
    printf("%*.s%s", 2 * depth, "", prefix);
    switch (node->kind)
    {
    case NK_NUM:
        printf("NUM(%d)\n", node->val);
        return;
    case NK_ADD:
        puts("ADD");
        break;
    case NK_SUB:
        puts("SUB");
        break;
    case NK_MUL:
        puts("MUL");
        break;
    case NK_DIV:
        puts("DIV");
        break;
    case NK_NEG:
        puts("NEG");
        break;
    default:
        printf("INVALID_NODE_KIND=%d\n", node->kind);
    }

    if (node->lhs && node->rhs)
    {
        dump_node(node->lhs, depth + 1, "LHS: ");
        dump_node(node->rhs, depth + 1, "RHS: ");
        return;
    }
    if (node->lhs)
    {
        dump_node(node->lhs, depth + 1, "OPERAND: ");
    }
}

void dump_nodes(Node *node)
{
    puts("----NODES----");
    while (node)
    {
        dump_node(node, 0, "");
        node = node->next;
    }
    puts("-------------");
}
