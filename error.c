#include <stdio.h>
#include <stdlib.h>

#include "tycc.h"

void verror_at(char *input, char *loc, char *fmt, va_list ap)
{
    int pos = loc - input;
    fprintf(stderr, "%s\n", input);
    fprintf(stderr, "%*s", pos, ""); // Print pos spaces.
    fprintf(stderr, "^ ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");

    exit(1);
}

void error_at(char *input, char *loc, char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    verror_at(input, loc, fmt, ap);
}
