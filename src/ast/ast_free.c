#include "ast_free.h"

#include <stdio.h>
#include <stdlib.h>

static void free_if(struct ast *ast)
{
    if (ast->data->ast_if->condition)
        free_node(ast->data->ast_if->condition);
    if (ast->data->ast_if->then)
        free_node(ast->data->ast_if->then);
    if (ast->data->ast_if->else_body)
        free_node(ast->data->ast_if->else_body);
    free(ast->data->ast_if);
}

static void free_list(struct ast *ast)
{
    for (size_t i = 0; i < ast->data->ast_list->size; i++)
        free_node(ast->data->ast_list->cmd_if[i]);
    free(ast->data->ast_list->cmd_if);
    free(ast->data->ast_list);
}

static void free_sp_cmd(struct ast *ast)
{
    for (size_t i = 0; i < ast->data->ast_sp_cmd->size_prefix; i++)
        free_node(ast->data->ast_sp_cmd->cmd_prefix[i]);
    for (size_t i = 0; i < ast->data->ast_sp_cmd->size_element; i++)
        free_node(ast->data->ast_sp_cmd->cmd_element[i]);
    free(ast->data->ast_sp_cmd->word);
    free(ast->data->ast_sp_cmd->cmd_prefix);
    free(ast->data->ast_sp_cmd->cmd_element);
}
    
static void free_cmd(struct ast *ast)
{
    vector_destroy(ast->data->ast_cmd->arg);
    free(ast->data->ast_cmd);
}

static void free_prefix(struct ast *ast)
{
    if (ast->data->ast_prefix->assign_word)
        free(ast->data->ast_prefix->assign_word);
    if (ast->data->ast_prefix->redir)
        free_node(ast->data->ast_prefix->redir);
    free(ast->data->ast_prefix);
}

static void free_redir(struct ast *ast)
{
    if (ast->data->ast_redir->exit_file)
        free(ast->data->ast_redir->exit_file);
    if (ast->data->ast_redir->io_number)
        free(ast->data->ast_redir->io_number);
    free(ast->data->ast_redir);
}

static void free_while(struct ast *ast)
{
    if (ast->data->ast_while->condition)
        free_node(ast->data->ast_while->condition);
    if (ast->data->ast_while->while_body)
        free_node(ast->data->ast_while->while_body);
    free(ast->data->ast_while);
}

static void free_until(struct ast *ast)
{
    if (ast->data->ast_until->condition)
        free_node(ast->data->ast_until->condition);
    if (ast->data->ast_until->until_body)
        free_node(ast->data->ast_until->until_body);
    free(ast->data->ast_until);
}

static void free_for(struct ast *ast)
{
    if (ast->data->ast_for->arg)
        free(ast->data->ast_for->arg);
    if (ast->data->ast_for->for_list)
        free_node(ast->data->ast_for->for_list);
    vector_destroy(ast->data->ast_cmd->arg);
    free(ast->data->ast_for);
}

static void free_prefix(struct ast *ast)
{
    if (ast->data->ast_prefix->assign_word)
        free(ast->data->ast_prefix->assign_word);
    if (ast->data->ast_prefix->redir)
        free_node(ast->data->ast_prefix->redir);
    free(ast->data->ast_prefix);
}

static void free_element(struct ast *ast)
{
    if (ast->data->ast_element->word)
        free(ast->data->ast_element->word);
    if (ast->data->ast_element->redir)
        free_node(ast->data->ast_element->redir);
    free(ast->data->ast_element);
}

/*
static void free_for(struct ast *ast)
{
    //TODO
}

static void free_while(struct ast *ast)
{
    //TODO
}

static void free_until(struct ast *ast)
{
    //TODO
}

static void free_and(struct ast *ast)
{
    //TODO
}

static void free_or(struct ast *ast)
{
    //TODO
}

static void free_not(struct ast *ast)
{
    //TODO
}

static void free_redirect(struct ast *ast)
{
    //TODO
}

static void free_pipe(struct ast *ast)
{
    //TODO
}*/

void free_node(struct ast *ast)
{
    if (!ast)
        return;
    if (ast->type == AST_IF)
        free_if(ast);
    else if (ast->type == AST_WHILE)
        free_while(ast);
    else if (ast->type == AST_FOR)
        free_for(ast);
    else if(ast->type == AST_UNTIL)
        free_until(ast);
    else if (ast->type == AST_LIST)
        free_list(ast);
    else if (ast->type == AST_CMD)
        free_cmd(ast);
    else if (ast->type == AST_PREFIX)
        free_prefix(ast);
    else if (ast->type == AST_REDIR)
        free_redir(ast);
    else if (ast->type == AST_SP_CMD)
        free_sp_cmd(ast);
    else if (ast->type == AST_ELEMENT)
        free_element(ast);
    free(ast->data);
    free(ast);
}

// ADD NEW AST FREE HERE