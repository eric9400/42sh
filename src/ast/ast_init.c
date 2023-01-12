#include "ast_init.h"

#include <stdio.h>
#include <stdlib.h>

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

/*
static struct ast_for *init_for(void)
{
    //TODO
    return NULL;
}

static struct ast_while *init_while(void)
{
    //TODO
    return NULL;
}

static struct ast_until *init_until(void)
{
    //TODO
    return NULL;
}

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

static struct ast_redirect *init_redirect(void)
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