#ifndef AST_H
#define AST_H

#include "vector.h"

enum ast_type
{
    AST_LIST,
    AST_IF,
    AST_CMD,

    AST_FOR,
    AST_WHILE,
    AST_UNTIL,

    AST_AND,
    AST_OR,
    AST_NOT,

    AST_REDIR,
    AST_PIPE,
};

enum redir_type
{
    S_RIGHT,    // >
    S_LEFT,     // <

    D_RIGHT,    // >>
    
    RIGHT_AND,  // >&
    LEFT_AND,   // <&
    
    RIGHT_PIP,  // >|
    LEFT_RIGHT  // <>
};

struct ast_list
{
    size_t size;
    size_t capacity;
    struct ast **cmd_if;//changer plus tard
};

struct ast_cmd
{
    struct vector *arg;
    struct ast_list *redir;
};

struct ast_if
{
    struct ast *condition;
    struct ast *then;
    struct ast *else_body;
};

struct ast_for
{
    char *var;
    struct vector *arg;
    struct ast *for_list;
};

struct ast_while
{
    struct ast *condition;
    struct ast *while_body;
};

struct ast_until
{
    struct ast *condition;
    struct ast *until_body;
};

struct ast_and
{
    struct ast *left;
    struct ast *right;
};

struct ast_or
{
    struct ast *left;
    struct ast *right;
};

struct ast_not
{
    struct ast *node;
};

struct ast_redir
{
    int io_number;
    char *exit_file;
    enum redir_type type;
};

struct ast_pipe
{
    struct ast *left;
    struct ast *right;
};


union ast_union
{
    struct ast_cmd *ast_cmd;
    struct ast_list *ast_list;
    struct ast_if *ast_if;

    struct ast_for *ast_for;
    struct ast_while *ast_while;
    struct ast_until *ast_until;

    struct ast_and *ast_and;
    struct ast_or *ast_or;
    struct ast_not *ast_not;

    struct ast_redir *ast_redir;
    struct ast_pipe *ast_pipe;
};

struct ast
{
    enum ast_type type;
    union ast_union *data;
};

void add_to_list(struct ast_list *list, struct ast *node);
struct ast *convert_node_ast(enum ast_type type, void *node);

#endif /* AST_H */