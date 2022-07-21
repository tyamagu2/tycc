#include <stdarg.h>

//
// Tokenizer
//

typedef enum
{
    TK_PUNCT,
    TK_NUM,
    TK_EOF,
} TokenKind;

typedef struct Token Token;

struct Token
{
    TokenKind kind;
    Token *next;
    char *str;
    char len;

    int val; // Value of the token for TK_NUM.
};

Token *tokenize(char *in);

//
// Parser
//

typedef enum
{
    NK_NUM,
    NK_ADD,
    NK_SUB,
    NK_MUL,
    NK_DIV,
} NodeKind;

typedef struct Node Node;

struct Node
{
    NodeKind kind;
    Node *next;
    Token *token; // Representative token

    Node *lhs; // Left-hand side
    Node *rhs; // Right-hand side

    // For NK_NUM
    int val;
};

Node *parse(Token *tok);

//
// Codegen
//

void codegen(Node *node);

//
// Dumper
//

void dump_tokens(Token *tok);
void dump_nodes(Node *node);

//
// Error reporting
//

void verror_at(char *input, char *loc, char *fmt, va_list ap);
void error_at(char *input, char *loc, char *fmt, ...);
