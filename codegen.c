#include <stdarg.h>
#include <stdio.h>

#include "tycc.h"

static void println(char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vprintf(fmt, ap);
    puts("");
}

void push(char *s)
{
    println("  push %s", s);
}

void pop(char *s)
{
    println("  pop %s", s);
}

void gen_expr(Node *node)
{
    switch (node->kind)
    {
    case NK_NUM:
        println("  push %d", node->val);
        return;
    }

    // binary
    gen_expr(node->lhs);
    gen_expr(node->rhs);
    pop("rdi");
    pop("rax");

    switch (node->kind)
    {
    case NK_ADD:
        println("  add rax, rdi");
        break;
    case NK_SUB:
        println("  sub rax, rdi");
        break;
    }

    push("rax");
}

void codegen(Node *node)
{
    println(".intel_syntax noprefix");
    println(".globl main");
    println("main:");

    while (node)
    {
        gen_expr(node);
        pop("rax");
        node = node->next;
    }

    println("  ret");
}