#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tycc.h"

// For error reporting.
static Token *_start;

char *node_kind_name(NodeKind kind)
{
    switch (kind)
    {
    case NK_NUM:
        return "NUM";
    case NK_ADD:
        return "ADD";
    case NK_SUB:
        return "SUB";
    case NK_MUL:
        return "MUL";
    case NK_DIV:
        return "DIV";
    case NK_NEG:
        return "NEG";
    case NK_EXPR_STMT:
        return "EXPR_STMT";
    case NK_ASSIGN:
        return "ASSIGN";
    case NK_LVAR:
        return "LVAR";
    case NK_EQ:
        return "EQ";
    case NK_NE:
        return "NE";
    case NK_LT:
        return "LT";
    case NK_LE:
        return "LE";
    case NK_GT:
        return "GT";
    case NK_GE:
        return "GE";
    case NK_RETURN:
        return "RETURN";
    case NK_IF:
        return "IF";
    case NK_FOR:
        return "FOR";
    case NK_WHILE:
        return "WHILE";
    case NK_BLOCK:
        return "BLOCK";
    case NK_FUNCCALL:
        return "FUNCCALL";
    }

    return "UNKNOWN";
}

bool equal_keyword(Token *tok, char *kw)
{
    if (tok->kind != TK_KEYWORD)
    {
        return false;
    }
    return equal(tok, kw);
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

Token *consume_keyword(Token *tok, Token **end, char *kw)
{
    if (tok->kind != TK_KEYWORD)
    {
        return NULL;
    }
    return consume(tok, end, kw);
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

void expect_keyword(Token *tok, Token **end, char *kw)
{
    if (tok->kind != TK_KEYWORD)
    {
        error_token(tok, "expected a keyword");
    }
    if (!consume(tok, end, kw))
    {
        error_token(tok, "expected %s", kw);
    }
}

Token *expect_ident(Token *tok, Token **end)
{
    if (tok->kind != TK_IDENT)
    {
        error_token(tok, "expected an ident");
    }
    Token *ret = tok;
    *end = tok->next;
    return ret;
}

Node *new_node(NodeKind kind, Token *tok)
{
    Node *node = calloc(1, sizeof(Node));
    node->kind = kind;
    node->token = tok;
    return node;
}

Node *new_unary(NodeKind kind, Token *tok, Node *expr)
{
    Node *node = new_node(kind, tok);
    node->lhs = expr;
    return node;
}

Node *new_binary(NodeKind kind, Token *tok, Node *lhs, Node *rhs)
{
    Node *node = new_node(kind, tok);
    node->lhs = lhs;
    node->rhs = rhs;
    return node;
}

Node *new_num(Token *tok, int val)
{
    Node *node = new_node(NK_NUM, tok);
    node->val = val;
    return node;
}

// locals is a list of local variables created during parsing.
LVar *locals;

static LVar *new_lvar(Token *tok)
{
    LVar *lvar = calloc(1, sizeof(LVar));
    lvar->name = tok->str;
    lvar->len = tok->len;

    // Prepend lvar to locals.
    lvar->next = locals;
    locals = lvar;
    return lvar;
}

static LVar *find_lvar(Token *tok)
{
    for (LVar *lvar = locals; lvar; lvar = lvar->next)
    {
        if (lvar->len == tok->len && memcmp(lvar->name, tok->str, tok->len) == 0)
        {
            return lvar;
        }
    }
    return NULL;
}

static Node *new_lvar_node(Token *tok)
{
    Node *node = new_node(NK_LVAR, tok);
    LVar *lvar = find_lvar(tok);
    if (lvar)
    {
        node->lvar = lvar;
        return node;
    }
    lvar = new_lvar(tok);
    node->lvar = lvar;
    return node;
}

char *strndup(const char *src, size_t len)
{
    char *dst = malloc(len + 1);
    dst['0'];
    memcpy(dst, src, len);
    return dst;
}

Node *funcdecl(Token *tok, Token **end);
Node *stmt(Token *tok, Token **end);
Node *if_stmt(Token *tok, Token **end);
Node *for_stmt(Token *tok, Token **end);
Node *while_stmt(Token *tok, Token **end);
Node *compound_stmt(Token *tok, Token **end);
Node *expr(Token *tok, Token **end);
Node *assign(Token *tok, Token **end);
Node *equality(Token *tok, Token **end);
Node *relational(Token *tok, Token **end);
Node *add(Token *tok, Token **end);
Node *mul(Token *tok, Token **end);
Node *unary(Token *tok, Token **end);
Node *primary(Token *tok, Token **end);
Node *funccall(Token *tok, Token **end);

// funcdecl
//  = ident "(" ")" "{" stmt* "}"
Node *funcdecl(Token *tok, Token **end)
{
    locals = NULL;

    Token *nt = expect_ident(tok, &tok);
    Node *node = new_node(NK_FUNCDECL, nt);
    node->funcname = strndup(nt->str, nt->len);

    expect(tok, &tok, "(");
    expect(tok, &tok, ")");
    expect(tok, &tok, "{");

    Node head = {};
    Node *cur = &head;
    while (!equal(tok, "}"))
    {
        cur = cur->next = stmt(tok, &tok);
    }

    expect(tok, &tok, "}");

    node->body = head.next;
    node->locals = locals;

    *end = tok;
    return node;
}

// stmt
//  = "return" expr ";"
//  | if_stmt
//  | for_stmt
//  | while_stmt
//  | "{" stmt* "}"
//  | expr? ";"
Node *stmt(Token *tok, Token **end)
{
    if (consume_keyword(tok, &tok, "return"))
    {
        Node *node = new_unary(NK_RETURN, tok, expr(tok, &tok));
        expect(tok, &tok, ";");
        *end = tok;
        return node;
    }

    if (equal_keyword(tok, "if"))
    {
        return if_stmt(tok, end);
    }
    if (equal_keyword(tok, "for"))
    {
        return for_stmt(tok, end);
    }
    if (equal_keyword(tok, "while"))
    {
        return while_stmt(tok, end);
    }

    if (equal(tok, "{"))
    {
        return compound_stmt(tok, end);
    }

    if (equal(tok, ";"))
    {
        Node *node = new_node(NK_BLOCK, tok);
        consume(tok, &tok, ";");
        *end = tok;
        return node;
    }

    Node *node = new_unary(NK_EXPR_STMT, tok, expr(tok, &tok));
    expect(tok, &tok, ";");
    *end = tok;
    return node;
}

// if_stmt
//  = "if" "(" expr ")" stmt ("else" stmt)?
Node *if_stmt(Token *tok, Token **end)
{
    Node *node = new_node(NK_IF, tok);

    expect_keyword(tok, &tok, "if");
    expect(tok, &tok, "(");
    node->cond = expr(tok, &tok);
    expect(tok, &tok, ")");
    node->then = stmt(tok, &tok);
    if (consume_keyword(tok, &tok, "else"))
    {
        node->els = stmt(tok, &tok);
    }

    *end = tok;
    return node;
}

// for_stmt
//  = "for" "(" expr? ";" expr? ";" expr? ")" stmt
Node *for_stmt(Token *tok, Token **end)
{
    Node *node = new_node(NK_FOR, tok);

    expect_keyword(tok, &tok, "for");

    expect(tok, &tok, "(");

    if (!equal(tok, ";"))
    {
        node->init = expr(tok, &tok);
    }
    expect(tok, &tok, ";");

    if (!equal(tok, ";"))
    {
        node->cond = expr(tok, &tok);
    }
    expect(tok, &tok, ";");

    if (!equal(tok, ")"))
    {
        node->inc = expr(tok, &tok);
    }

    expect(tok, &tok, ")");

    node->then = stmt(tok, &tok);

    *end = tok;
    return node;
}

// while_stmt
//  = "while" "(" expr ")" stmt
Node *while_stmt(Token *tok, Token **end)
{
    Node *node = new_node(NK_WHILE, tok);

    expect_keyword(tok, &tok, "while");
    expect(tok, &tok, "(");
    node->cond = expr(tok, &tok);
    expect(tok, &tok, ")");
    node->then = stmt(tok, &tok);

    *end = tok;
    return node;
}

Node *compound_stmt(Token *tok, Token **end)
{
    Node *node = new_node(NK_BLOCK, tok);
    Node head = {};
    Node *cur = &head;

    expect(tok, &tok, "{");
    while (!equal(tok, "}"))
    {
        cur = cur->next = stmt(tok, &tok);
    }
    expect(tok, &tok, "}");

    node->body = head.next;

    *end = tok;
    return node;
}

// expr
//  = assign
Node *expr(Token *tok, Token **end)
{
    return assign(tok, end);
}

// assign
//  = equality (= assign)?
Node *assign(Token *tok, Token **end)
{
    Node *node = equality(tok, &tok);
    if (consume(tok, &tok, "="))
    {
        node = new_binary(NK_ASSIGN, tok, node, assign(tok, &tok));
    }
    *end = tok;
    return node;
}

// equality
//  = relational ("==" relational | "!=" relational)*
Node *equality(Token *tok, Token **end)
{
    Node *node = relational(tok, &tok);
    for (;;)
    {
        if (consume(tok, &tok, "=="))
        {
            node = new_binary(NK_EQ, tok, node, relational(tok, &tok));
            continue;
        }
        if (consume(tok, &tok, "!="))
        {
            node = new_binary(NK_NE, tok, node, relational(tok, &tok));
            continue;
        }
        break;
    }

    *end = tok;
    return node;
}

// relational
//  = add ("<" add | "<=" add | ">" add | ">=" add)*
Node *relational(Token *tok, Token **end)
{
    Node *node = add(tok, &tok);
    for (;;)
    {
        if (consume(tok, &tok, "<"))
        {
            node = new_binary(NK_LT, tok, node, add(tok, &tok));
        }
        else if (consume(tok, &tok, "<="))
        {
            node = new_binary(NK_LE, tok, node, add(tok, &tok));
        }
        if (consume(tok, &tok, ">"))
        {
            node = new_binary(NK_GT, tok, node, add(tok, &tok));
        }
        else if (consume(tok, &tok, ">="))
        {
            node = new_binary(NK_GE, tok, node, add(tok, &tok));
        }
        else
        {
            break;
        }
    }

    *end = tok;
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
//  = unary ("*" unary | "/" unary)*
Node *mul(Token *tok, Token **end)
{
    Node *node = unary(tok, &tok);

    for (;;)
    {
        if (consume(tok, &tok, "*"))
        {
            node = new_binary(NK_MUL, tok, node, unary(tok, &tok));
            continue;
        }
        if (consume(tok, &tok, "/"))
        {
            node = new_binary(NK_DIV, tok, node, unary(tok, &tok));
            continue;
        }
        break;
    }

    *end = tok;
    return node;
}

// unary
//  = ("+" | "-")? unary
//  | primary
Node *unary(Token *tok, Token **end)
{
    if (consume(tok, &tok, "+"))
    {
        return unary(tok, end);
    }
    if (consume(tok, &tok, "-"))
    {
        return new_unary(NK_NEG, tok, unary(tok, end));
    }
    return primary(tok, end);
}

// primary
//  = num
//  | ident
//  | funccall
//  | "(" expr ")"
Node *primary(Token *tok, Token **end)
{
    if (tok->kind == TK_NUM)
    {
        Node *node = new_num(tok, tok->val);
        *end = tok->next;
        return node;
    }
    if (tok->kind == TK_IDENT)
    {
        if (equal(tok->next, "("))
        {
            return funccall(tok, end);
        }

        Node *node = new_lvar_node(tok);
        *end = tok->next;
        return node;
    }

    expect(tok, &tok, "(");
    Node *node = expr(tok, &tok);
    expect(tok, &tok, ")");
    *end = tok;
    return node;
}

// funccall
//  = ident args?
//
// args
//  = "(" expr? ("," expr)* ")"
Node *funccall(Token *tok, Token **end)
{
    Token *nt = expect_ident(tok, &tok);
    Node *node = new_node(NK_FUNCCALL, nt);
    node->funcname = strndup(nt->str, nt->len);

    expect(tok, &tok, "(");

    Node head = {};
    Node *cur = &head;
    if (!equal(tok, ")"))
    {
        cur = cur->next = expr(tok, &tok);
    }
    while (!equal(tok, ")"))
    {
        expect(tok, &tok, ",");
        cur = cur->next = expr(tok, &tok);
    }

    expect(tok, &tok, ")");

    node->args = head.next;

    *end = tok;
    return node;
}

Program *parse(Token *tok)
{
    _start = tok;

    Node head;
    Node *cur = &head;
    while (tok->kind != TK_EOF)
    {
        cur = cur->next = funcdecl(tok, &tok);
    }

    Program *prg = calloc(1, sizeof(Program));
    prg->nodes = head.next;

    return prg;
}
