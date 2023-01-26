#include "ast_init.h"

#include <stdio.h>
#include <stdlib.h>

#define SIZE 100

static struct ast_cmd *init_cmd(size_t capacity);
static struct ast_list *init_list(size_t capacity);
static struct ast_if *init_if(size_t capacity);
static struct ast_for *init_for(size_t capacity);
static struct ast_while *init_while(size_t capacity);
static struct ast_until *init_until(size_t capacity);
static struct ast_and *init_and(void);
static struct ast_or *init_or(void);
static struct ast_not *init_not(void);
static struct ast_redir *init_redir(void);
static struct ast_pipe *init_pipe(void);
static struct ast_func *init_function(size_t capacity);
static struct ast_subshell *init_subshell(void);

// 27 lines
void *init_ast(enum ast_type type)
{
    if (type == AST_CMD)
        return init_cmd(SIZE);
    else if (type == AST_IF)
        return init_if(SIZE);
    else if (type == AST_LIST)
        return init_list(SIZE);
    else if (type == AST_FOR)
        return init_for(SIZE);
    else if (type == AST_WHILE)
        return init_while(SIZE);
    else if (type == AST_UNTIL)
        return init_until(SIZE);
    else if (type == AST_AND)
        return init_and();
    else if (type == AST_OR)
        return init_or();
    else if (type == AST_NOT)
        return init_not();
    else if (type == AST_REDIR)
        return init_redir();
    else if (type == AST_PIPE)
        return init_pipe();
    else if (type == AST_FUNC)
        return init_function(SIZE);
    else if (type == AST_SUBSHELL)
        return init_subshell();
    return NULL;
    // ADD NEW AST INIT HERE
}

static struct ast_cmd *init_cmd(size_t capacity)
{
    struct ast_cmd *ast_cmd = calloc(1, sizeof(struct ast_cmd));
    ast_cmd->arg = vector_init(10);
    ast_cmd->redir = init_list(capacity);
    return ast_cmd;
}

static struct ast_list *init_list(size_t capacity)
{
    struct ast_list *ast_list = calloc(1, sizeof(struct ast_list));
    ast_list->cmd_if = calloc(capacity, sizeof(struct ast *));
    ast_list->size = 0;
    ast_list->capacity = capacity;
    return ast_list;
}

static struct ast_if *init_if(size_t capacity)
{
    struct ast_if *ast_if = calloc(1, sizeof(struct ast_if));
    ast_if->condition = NULL;
    ast_if->then = NULL;
    ast_if->else_body = NULL;
    ast_if->redir = init_list(capacity);
    return ast_if;
}

static struct ast_while *init_while(size_t capacity)
{
    struct ast_while *ast_while = calloc(1, sizeof(struct ast_while));
    ast_while->condition = NULL;
    ast_while->while_body = NULL;
    ast_while->redir = init_list(capacity);
    return ast_while;
}

static struct ast_until *init_until(size_t capacity)
{
    struct ast_until *ast_until = calloc(1, sizeof(struct ast_until));
    ast_until->condition = NULL;
    ast_until->until_body = NULL;
    ast_until->redir = init_list(capacity);
    return ast_until;
}

static struct ast_redir *init_redir(void)
{
    struct ast_redir *ast_redir = calloc(1, sizeof(struct ast_redir));
    ast_redir->io_number = -1;
    ast_redir->exit_file = NULL;
    return ast_redir;
}

static struct ast_for *init_for(size_t capacity)
{
    struct ast_for *ast_for = calloc(1, sizeof(struct ast_for));
    ast_for->var = NULL;
    ast_for->for_list = NULL;
    ast_for->arg = vector_init(10);
    ast_for->redir = init_list(capacity);
    return ast_for;
}

static struct ast_and *init_and(void)
{
    struct ast_and *ast_and = calloc(1, sizeof(struct ast_and));
    ast_and->left = NULL;
    ast_and->right = NULL;
    return ast_and;
}

static struct ast_or *init_or(void)
{
    struct ast_or *ast_or = calloc(1, sizeof(struct ast_or));
    ast_or->left = NULL;
    ast_or->right = NULL;
    return ast_or;
}

static struct ast_not *init_not(void)
{
    struct ast_not *ast_not = calloc(1, sizeof(struct ast_not));
    ast_not->node = NULL;
    return ast_not;
}

static struct ast_pipe *init_pipe(void)
{
    struct ast_pipe *ast_pipe = calloc(1, sizeof(struct ast_pipe));
    ast_pipe->left = NULL;
    ast_pipe->right = NULL;
    return ast_pipe;
}

static struct ast_func *init_function(size_t capacity)
{
    struct ast_func *ast_func = calloc(1, sizeof(struct ast_func));
    ast_func->func = NULL;
    ast_func->name = NULL;
    ast_func->redir = init_list(capacity);
    return ast_func;
}

static struct ast_subshell *init_subshell(void)
{
    struct ast_subshell *ast_subshell = calloc(1, sizeof(struct ast_subshell));
    ast_subshell->sub = NULL;
    return ast_subshell;
}
// ADD NEW AST INIT HERE
