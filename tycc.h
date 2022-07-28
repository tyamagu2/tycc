#include <stdarg.h>
#include <stdbool.h>

//
// Tokenizer
//

typedef enum
{
    TK_EOF,
    TK_PUNCT,
    TK_NUM,
    TK_IDENT,
    TK_KEYWORD,
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

bool equal(Token *tok, char *op);

//
// Parser
//

typedef struct LVar LVar;

struct LVar
{
    LVar *next;
    char *name;
    int len;
    int offset; // Offset from rbp. To be set during code generation.
};

typedef enum
{
    NK_NUM,       // Integer
    NK_ADD,       // +
    NK_SUB,       // -
    NK_MUL,       // *
    NK_DIV,       // /
    NK_NEG,       // Unary -
    NK_EXPR_STMT, // Expression statement
    NK_ASSIGN,    // Assignment
    NK_LVAR,      // Local variable
    NK_EQ,        // ==
    NK_NE,        // !=
    NK_LT,        // <
    NK_LE,        // <=
    NK_GT,        // >
    NK_GE,        // >=
    NK_RETURN,    // return stmt
    NK_IF,        // if stmt
    NK_FOR,       // for stmt
    NK_WHILE,     // while stmt
    NK_BLOCK,     // Block
    NK_FUNCCALL,  // Function call
} NodeKind;

typedef struct Node Node;

struct Node
{
    NodeKind kind;
    Node *next;
    Token *token; // Representative token

    Node *lhs; // Left-hand side
    Node *rhs; // Right-hand side

    // For NK_BLOCK
    Node *body;

    // For NK_IF, NK_FOR and NK_WHILE
    Node *init;
    Node *cond;
    Node *then;
    Node *els;
    Node *inc;

    // For NK_NUM
    int val;

    // For NK_LVAR
    LVar *lvar;

    // For NK_FUNCCALL
    char *funcname;
    Node *args;
};

typedef struct Function
{
    Node *body;
    LVar *locals;
} Function;

Function *parse(Token *tok);
char *node_kind_name(NodeKind kind);

//
// Codegen
//

void codegen(Function *prog);

//
// Dumper
//

void dump_tokens(Token *tok);
void dump_nodes(Function *prog);

//
// Error reporting
//

void verror_at(char *input, char *loc, char *fmt, va_list ap);
void error_at(char *input, char *loc, char *fmt, ...);
