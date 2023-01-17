#include "ast_print.h"

#include <stdio.h>
#include <stdlib.h>

static void print_if(struct ast *ast, int tab);
static void print_list(struct ast *ast, int tab);
static void print_cmd(struct ast *ast, int tab);
static void print_for(struct ast *ast, int tab);
static void print_while(struct ast *ast, int tab);
static void print_until(struct ast *ast, int tab);
static void print_and(struct ast *ast, int tab);
static void print_or(struct ast *ast, int tab);
static void print_not(struct ast *ast, int tab);
static void print_redirect(struct ast *ast);
static void print_pipe(struct ast *ast, int tab);

static void print_tab(int tab)
{
    for (int t = 0; t < tab; t++)
        printf("    ");
}

void ugly_print(struct ast *ast, int tab)
{
    if (!ast)
        printf("NULL");
    if (ast->type == AST_IF)
        print_if(ast, tab);
    else if (ast->type == AST_WHILE)
        print_while(ast, tab);
    else if (ast->type == AST_FOR)
        print_for(ast, tab);
    else if(ast->type == AST_UNTIL)
        print_until(ast, tab);
    else if (ast->type == AST_LIST)
        print_list(ast, tab);
    else if (ast->type == AST_CMD)
        print_cmd(ast, tab);
    else if (ast->type == AST_REDIR)
        print_redirect(ast);
    else if (ast->type == AST_AND)
        print_and(ast, tab);
    else if (ast->type == AST_OR)
        print_or(ast, tab);
    else if (ast->type == AST_PIPE)
        print_pipe(ast, tab);
    else if (ast->type == AST_NOT)
        print_not(ast, tab);
}

static void print_if(struct ast *ast, int tab)
{
    print_tab(tab);
    printf("IF:\n");
    ugly_print(ast->data->ast_if->condition, tab + 1);
    print_tab(tab);
    printf("THEN:\n");
    ugly_print(ast->data->ast_if->then, tab + 1);
    printf("\n");
    if (ast->data->ast_if->else_body)
    {
        print_tab(tab);
        printf("ELSE:\n");
        ugly_print(ast->data->ast_if->else_body, tab + 1);
        printf("\n");
    }
}

static void print_list(struct ast *ast, int tab)
{
    print_tab(tab);
    printf("LIST:\n");
    for (size_t i = 0; i < ast->data->ast_list->size; i++)
    {
        ugly_print(ast->data->ast_list->cmd_if[i], tab + 1);
    }
}

static void print_cmd(struct ast *ast, int tab)
{
    print_tab(tab);
    struct vector *v = ast->data->ast_cmd->arg;
    if (v->size == 0)
        return;
    if (v->size == 1)
    {
        printf(" %s", v->data[0]);
        return;
    }
    for (size_t i = 0; i < v->size - 1; i++)
        printf(" %s", v->data[i]);
    printf(";\n");
    for (size_t i = 0; i < ast->data->ast_cmd->redir->size; i++)
    {
        ugly_print(ast->data->ast_cmd->redir->cmd_if[i], tab + 1);
    }
}

static void print_for(struct ast *ast, int tab)
{
    print_tab(tab);
    printf("FOR %s IN", ast->data->ast_for->var);
    for (size_t i = 0; i < ast->data->ast_for->arg->size; i++)
        printf(" %s", ast->data->ast_for->arg->data[i]);
    printf(":\n");
    ugly_print(ast->data->ast_for->for_list, tab + 1);
    printf("\n");
}

static void print_while(struct ast *ast, int tab)
{
    print_tab(tab);
    printf("WHILE:\n");
    ugly_print(ast->data->ast_while->condition, tab + 1);
    print_tab(tab);
    printf("DO:\n");
    ugly_print(ast->data->ast_while->while_body, tab + 1);
    printf("\n");
}

static void print_until(struct ast *ast, int tab)
{
    print_tab(tab);
    printf("UNTIL:\n");
    ugly_print(ast->data->ast_until->condition, tab + 1);
    print_tab(tab);
    printf("DO:\n");
    ugly_print(ast->data->ast_until->until_body, tab + 1);
    printf("\n");
}

static void print_and(struct ast *ast, int tab)
{
    ugly_print(ast->data->ast_and->left, tab);
    print_tab(tab);
    printf("&& ");
    ugly_print(ast->data->ast_and->right, 0);
}

static void print_or(struct ast *ast, int tab)
{
    ugly_print(ast->data->ast_or->left, tab);
    print_tab(tab);
    printf("|| ");
    ugly_print(ast->data->ast_or->right, 0);
}

static void print_not(struct ast *ast, int tab)
{
    print_tab(tab);
    printf("!");
    ugly_print(ast->data->ast_not->node, 0);
}

static void print_redirect(struct ast *ast)
{
    if (ast->data->ast_redir->io_number)
        printf("%d", ast->data->ast_redir->io_number); 
    printf(">>>");
    if (ast->data->ast_redir->exit_file)
        printf("%s", ast->data->ast_redir->exit_file);
}

static void print_pipe(struct ast *ast, int tab)
{
    ugly_print(ast->data->ast_pipe->left, tab + 1);
    printf("| ");
    ugly_print(ast->data->ast_pipe->right, tab + 1);
}

// ADD NEW AST PRINT HERE
