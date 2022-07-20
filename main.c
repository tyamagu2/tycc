#include <stdbool.h>
#include <stdio.h>

#include "tycc.h"

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        fprintf(stderr, "wrong number of arguments");
        return 1;
    }

    bool dump = argc >= 3;

    Token *tok = tokenize(argv[1]);
    if (dump)
    {
        dump_tokens(tok);
    }

    Node *node = parse(tok);
    if (dump)
    {
        dump_nodes(node);
    }

    codegen(node);

    return 0;
}