#include "ast_init.h"

#include <stdio.h>
#include <stdlib.h>

#define SIZE 100

static struct ast_cmd *init_cmd(size_t capacity);
static struct ast_list *init_list(size_t capacity);
static struct ast_if *init_if(void);
static struct ast_for *init_for(void);
static struct ast_while *init_while(void);
static struct ast_until *init_until(void);
static struct ast_and *init_and(void);
static struct ast_or *init_or(void);
static struct ast_not *init_not(void);
static struct ast_redir *init_redir(void);
static struct ast_pipe *init_pipe(void);
static struct ast_prefix *init_prefix(void);
static struct ast_element *init_element(void);
static struct ast_sp_cmd *init_sp_cmd(void);
static struct ast_sh_cmd *init_sh_cmd(void);

void *init_ast(enum ast_type type)
{
    if (type == AST_CMD)
        return init_cmd(SIZE);
    else if (type == AST_IF)
        return init_if();
    else if (type == AST_LIST)
        return init_list(SIZE);
    else if (type == AST_FOR)
        return init_for();
    else if (type == AST_WHILE)
        return init_while();
    else if (type == AST_UNTIL)
        return init_until();
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
    /*else if (type == AST_PREFIX)
        return init_prefix();
    else if (type == AST_ELEMENT)
        return init_element();
    else if (type == AST_SP_CMD)
        return init_sp_cmd();
    else if (type == AST_SH_CMD)
        return init_sh_cmd();*/
    return NULL;
    // ADD NEW AST INIT HERE
}

/*
static struct ast_sp_cmd *init_sp_cmd(void)
{
    struct ast_sp_cmd *ast_cmd = calloc(1, sizeof(struct ast_sp_cmd));
    ast_cmd->word = NULL;
    ast_cmd->cmd_prefix = calloc(10, sizeof(struct ast_prefix *)); //CHANGE THIS WITH REALLOC
    ast_cmd->cmd_element = calloc(10, sizeof(struct ast_element *)); //CHANGE THIS WITH REALLOC
    ast_cmd->size_prefix = 0;
    ast_cmd->size_element = 0;
    return ast_cmd;
}

static struct ast_sh_cmd *init_sh_cmd(void)
{
    struct ast_sh_cmd *cmd = calloc(1, sizeof(struct ast_sh_cmd));
    cmd->cmd = NULL;
    cmd->size_redir = 0;
    cmd->redir = NULL;
    return cmd;
}*/

static struct ast_cmd *init_cmd(size_t capacity)
{
    struct ast_cmd *ast_cmd = calloc(1, sizeof(struct ast_cmd));
    ast_cmd->arg = vector_init(10);
    struct ast_list *redir = init_list(capacity);
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

static struct ast_if *init_if(void)
{
    struct ast_if *ast_if = calloc(1, sizeof(struct ast_if));
    ast_if->condition = NULL;
    ast_if->then = NULL;
    ast_if->else_body = NULL;
    return ast_if;
}

static struct ast_while *init_while(void)
{
    struct ast_while *ast_while = calloc(1, sizeof(struct ast_while));
    ast_while->condition = NULL;
    ast_while->while_body = NULL;
    return ast_while;
}

static struct ast_until *init_until(void)
{
    struct ast_until *ast_until = calloc(1, sizeof(struct ast_until));
    ast_until->condition = NULL;
    ast_until->until_body = NULL;
    return ast_until;
}

/*
static struct ast_prefix *init_prefix(void)
{
    struct ast_prefix *ast_prefix = calloc(1, sizeof(struct ast_prefix));
    ast_prefix->assign_word = NULL;
    ast_prefix->redir = NULL;
    return ast_prefix;
}*/

static struct ast_redir *init_redir(void)
{
    struct ast_redir *ast_redir = calloc(1, sizeof(struct ast_redir));
    ast_redir->io_number = -1;
    ast_redir->exit_file = NULL;
    return ast_redir;
}

/*static struct ast_element *init_element(void)
{
    struct ast_element *ast_element = calloc(1, sizeof(struct ast_element));
    ast_element->word = NULL;
    ast_element->redir = NULL;
    return ast_element;
}*/

static struct ast_for *init_for(void)
{
    struct ast_for *ast_for = calloc(1, sizeof(struct ast_for));
    ast_for->var = NULL;
    ast_for->for_list = NULL;
    ast_for->arg = vector_init(10);
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
// ADD NEW AST INIT HERE
