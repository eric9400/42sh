#include "ast_init.h"

#include <stdio.h>
#include <stdlib.h>

struct ast_cmd *init_cmd(void);
struct ast_list *init_list(size_t capacity);
struct ast_if *init_if(void);
/*static struct ast_for *init_for(void);
static struct ast_while *init_while(void);
static struct ast_until *init_until(void);
static struct ast_and *init_and(void);
static struct ast_or *init_or(void);
static struct ast_not *init_not(void);
static struct ast_redirect *init_redirect(void);
static struct ast_redirect *init_pipe(void);*/

void *init_ast(enum ast_type type, size_t capacity)
{
    if (type == AST_CMD)
        return init_cmd();
    else if (type == AST_IF)
        return init_if();
    else if (type == AST_LIST)
        return init_list(capacity);
    return NULL;
    // ADD NEW AST INIT HERE

struct ast_sp_cmd *init_sp_cmd(void)
{
    struct ast_sp_cmd *ast_cmd = calloc(1, sizeof(struct ast_cmd));
    ast_cmd->word = NULL;
    ast_cmd->cmd_prefix = NULL;
    ast_cmd->cmd_element = NULL;
    ast_cmd->size_prefix = 0;
    ast_cmd->size_element = 0;
    return ast_cmd;
}

struct ast_sh_cmd *init_sh_cmd(void)
{
    struct ast_sh_cmd *cmd = calloc(1, sizeof(struct ast_sh_cmd));
    cmd->cmd = NULL;
    cmd->size_redir = 0;
    cmd->redir = NULL;
    return cmd;
}

struct ast_cmd *init_cmd(void)
{
    struct ast_cmd *ast_cmd = calloc(1, sizeof(struct ast_cmd));
    ast_cmd->arg = vector_init(10);
    return ast_cmd;
}

struct ast_list *init_list(size_t capacity)
{
    struct ast_list *ast_list = calloc(1, sizeof(struct ast_list));
    ast_list->cmd_if = calloc(capacity, sizeof(struct ast *));
    ast_list->size = 0;
    ast_list->capacity = capacity;
    return ast_list;
}

struct ast_if *init_if(void)
{
    struct ast_if *ast_if = calloc(1, sizeof(struct ast_if));
    ast_if->condition = NULL;
    ast_if->then = NULL;
    ast_if->else_body = NULL;
    return ast_if;
}

struct ast_while *init_while(void)
{
    struct ast_while *ast_while = calloc(1, sizeof(struct ast_while));
    ast_while->condition = NULL;
    ast_while->while_body = NULL;
    return ast_while;
}

struct ast_until *init_until(void)
{
    struct ast_until *ast_until = calloc(1, sizeof(struct ast_until));
    ast_until->condition = NULL;
    ast_until->until_body = NULL;
    return ast_until;
}

struct ast_prefix *init_prefix(void)
{
    struct ast_prefix *ast_prefix = calloc(1, sizeof(struct ast_prefix));
    ast_prefix->assign_word = NULL;
    ast_prefix->redir = NULL;
    return ast_prefix;
}

struct ast_redir *init_redir(void)
{
    struct ast_redir *ast_redir = calloc(1, sizeof(struct ast_redir));
    ast_redir->io_number = NULL;
    ast_redir->exit_file = NULL;
    return ast_redir;
}

struct ast_element *init_element(void)
{
    struct ast_element *ast_element = calloc(1, sizeof(struct ast_element));
    ast_element->word = NULL;
    ast_element->redir = NULL;
    return ast_element;
}

struct ast_for *init_for(void)
{
    struct ast_for *ast_for = calloc(1, sizeof(struct ast_for));
    ast_for->var = NULL;
    ast_for->for_list = NULL;
    ast_for->arg = vector_init(10);
    return ast_for;
}

struct ast_and *init_and(void)
{
    struct ast_and *ast_and = calloc(1, sizeof(struct ast_and));
    ast_and->left = NULL;
    ast_and->right = NULL;
    return ast_and;
}

struct ast_or *init_or(void)
{
    struct ast_or *ast_or = calloc(1, sizeof(struct ast_or));
    ast_or->left = NULL;
    ast_or->right = NULL;
    return ast_or;
}

struct ast_not *init_not(void)
{
    struct ast_not *ast_not = calloc(1, sizeof(struct ast_not));
    ast_not->node = NULL;
    return ast_not;
}

struct ast_pipe *init_pipe(void)
{
    struct ast_pipe *ast_pipe = calloc(1, sizeof(struct ast_pipe));
    ast_pipe->left = NULL;
    ast_pipe->right = NULL;
    return ast_pipe;
}

/*
static struct ast_and *init_and(void)
{
    //TODO
    return NULL;
}

static struct ast_or *init_or(void)
{
    //TODO
    return NULL;
}

static struct ast_not *init_not(void)
{
    //TODO
    return NULL;
}

static struct ast_redirect *init_pipe(void)
{
    //TODO
    return NULL;
}
*/
// ADD NEW AST INIT HERE