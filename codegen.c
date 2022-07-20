#include <stdarg.h>
#include <stdio.h>

static void println(char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vprintf(fmt, ap);
    puts("");
}

void codegen(char *in)
{
    println(".intel_syntax noprefix");
    println(".globl main");
    println("");
    println("main:");
    println("  mov rax, %s", in);
    println("  ret");
}
