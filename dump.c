#include <stdio.h>

#include "tycc.h"

static void dump_token(Token *tok)
{
    switch (tok->kind)
    {
    case TK_EOF:
        puts("EOF");
        return;
    case TK_PUNCT:
        printf("PUNCT");
        break;
    case TK_NUM:
        printf("NUM");
        break;
    case TK_IDENT:
        printf("IDENT");
        break;
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
    case NK_SUB:
    case NK_MUL:
    case NK_DIV:
    case NK_NEG:
    case NK_ASSIGN:
    case NK_EQ:
    case NK_NE:
    case NK_LT:
    case NK_LE:
    case NK_GT:
    case NK_GE:
        puts(node_kind_name(node->kind));
        break;
    case NK_EXPR_STMT:
        dump_node(node->lhs, depth, "EXPR_STMT: ");
        return;
    case NK_LVAR:
        printf("LVAR(%.*s)\n", node->token->len, node->token->str);
        return;
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

void dump_nodes(Function *prog)
{
    puts("----NODES----");
    Node *node = prog->body;
    while (node)
    {
        dump_node(node, 0, "");
        node = node->next;
    }
    puts("-------------");

    puts("----LVARS----");
    LVar *lvar = prog->locals;
    while (lvar)
    {
        printf("%.*s\n", lvar->len, lvar->name);
        lvar = lvar->next;
    }
    puts("-------------");
}
