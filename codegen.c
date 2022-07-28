#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "tycc.h"

static void error(char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

static void error_node(Node *node, char *msg)
{
    error("%s: %s(%.*s)", msg, node_kind_name(node->kind), node->token->len, node->token->str);
}

static int count()
{
    static int cnt;
    cnt++;
    return cnt;
}
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

void push_lvar_addr(Node *node)
{
    if (node->kind != NK_LVAR)
    {
        error_node(node, "the left hand side of an assignment is not a local variable");
    }

    println("  mov rax, rbp");
    println("  sub rax, %d", node->lvar->offset);
    push("rax");
}

static const char *argregs[] = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};

void gen_expr(Node *node)
{
    switch (node->kind)
    {
    case NK_NUM:
        println("  push %d", node->val);
        return;
    case NK_LVAR:
        push_lvar_addr(node);
        pop("rax");
        println("  mov rax, [rax]");
        push("rax");
        return;
    case NK_ASSIGN:
        push_lvar_addr(node->lhs);
        gen_expr(node->rhs);
        pop("rdi");
        pop("rax");
        println("  mov [rax], rdi");
        push("rdi");
        return;
    case NK_FUNCCALL:
    {
        int ai = 0;
        for (Node *arg = node->args; arg; arg = arg->next)
        {
            if (ai > sizeof(argregs)/sizeof(const char*)) {
                error_node(node, "too many arguments passed");
            }
            gen_expr(arg);
            pop("rax");
            println("  mov %s, rax", argregs[ai++]);
        }
        println("  mov rax, 0");
        println("  call %s", node->funcname);
        push("rax");
        return;
    }
    }

    if (node->lhs && node->rhs)
    {
        gen_expr(node->lhs);
        gen_expr(node->rhs);
        pop("rdi");
        pop("rax");
    }
    else if (node->lhs)
    {
        gen_expr(node->lhs);
        pop("rax");
    }

    switch (node->kind)
    {
    case NK_ADD:
        println("  add rax, rdi");
        break;
    case NK_SUB:
        println("  sub rax, rdi");
        break;
    case NK_MUL:
        println("  imul rax, rdi");
        break;
    case NK_DIV:
        println("  cqo");      // Set sign-extend of rax to rdx:rax.
        println("  idiv rdi"); // Signed divide rdx:rax by rdi.
        break;
    case NK_EQ:
    case NK_NE:
    case NK_LT:
    case NK_LE:
    case NK_GT:
    case NK_GE:
        println("  cmp rax, rdi");
        if (node->kind == NK_EQ)
        {
            println("  sete al");
        }
        else if (node->kind == NK_NE)
        {
            println("  setne al");
        }
        else if (node->kind == NK_LT)
        {
            println("  setl al");
        }
        else if (node->kind == NK_LE)
        {
            println("  setle al");
        }
        else if (node->kind == NK_GT)
        {
            println("  setg al");
        }
        else if (node->kind == NK_GE)
        {
            println("  setge al");
        }
        println("  movzb rax, al");
        break;
    case NK_NEG:
        println("  neg rax");
        break;
    default:
        error_node(node, "unknown expression");
    }

    push("rax");
}

void gen_stmt(Node *node)
{
    switch (node->kind)
    {
    case NK_RETURN:
        gen_expr(node->lhs);
        pop("rax");
        println("  mov rsp, rbp");
        pop("rbp"); // Restore rbp.
        println("  ret");
        return;
    case NK_IF:
    {
        int c = count();
        gen_expr(node->cond);
        pop("rax");
        println("  cmp rax, 0");
        println("  je .L.else%d", c);
        gen_stmt(node->then);
        println("  jmp .L.end%d", c);
        println(".L.else%d:", c);
        if (node->els)
        {
            gen_stmt(node->els);
        }
        println(".L.end%d:", c);
        return;
    }
    case NK_FOR:
    {
        if (node->init)
        {
            gen_expr(node->init);
        }
        int c = count();
        println(".L.begin%d:", c);
        if (node->cond)
        {
            gen_expr(node->cond);
            pop("rax");
            println("  cmp rax, 0");
            println("  je .L.end%d", c);
        }
        gen_stmt(node->then);
        if (node->inc)
        {
            gen_expr(node->inc);
        }
        println("  jmp .L.begin%d", c);
        println(".L.end%d:", c);
        return;
    }
    case NK_WHILE:
    {
        int c = count();
        println(".L.begin%d:", c);
        gen_expr(node->cond);
        pop("rax");
        println("  cmp rax, 0");
        println("  je .L.end%d", c);
        gen_stmt(node->then);
        println("  jmp .L.begin%d", c);
        println(".L.end%d:", c);
        return;
    }
    case NK_EXPR_STMT:
        gen_expr(node->lhs);
        return;
    case NK_BLOCK:
        for (Node *cur = node->body; cur; cur = cur->next)
        {
            gen_stmt(cur);
        }
        return;
    default:
        error_node(node, "unknown statement");
    }
}

static int assign_lvar_offset(Node *f)
{
    int offset = 0;
    for (LVar *v = f->locals; v; v = v->next)
    {
        offset += 8;
        v->offset = offset;
    }
    return offset;
}

void gen_funcdecl(Node *f)
{
    println("%s:", f->funcname);

    // Prologue
    push("rbp"); // rbp is a callee-saved register and its value must be preserved.
    println("  mov rbp, rsp");
    int offset = assign_lvar_offset(f);
    println("  sub rsp, %d", offset);

    Node *node = f->body;
    while (node)
    {
        gen_stmt(node);
        pop("rax");
        node = node->next;
    }

    // Epilogue
    println("  mov rsp, rbp");
    pop("rbp"); // Restore rbp.
    println("  ret");
}

void codegen(Program *prog)
{
    println(".intel_syntax noprefix");
    println(".globl main");

    for (Node *f = prog->nodes; f; f = f->next)
    {
        gen_funcdecl(f);
    }
}
