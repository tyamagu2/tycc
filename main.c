#include <stdio.h>

#include "tycc.h"

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        fprintf(stderr, "wrong number of arguments");
        return 1;
    }

    codegen(argv[1]);

    return 0;
}