#include "ast_print.h"

#include <stdio.h>
#include <stdlib.h>

static void print_if(struct ast *ast);
static void print_list(struct ast *ast);
static void print_cmd(struct ast *ast);
static void print_for(struct ast *ast);
static void print_while(struct ast *ast);
static void print_until(struct ast *ast);
static void print_and(struct ast *ast);
static void print_or(struct ast *ast);
static void print_not(struct ast *ast);
static void print_redirect(struct ast *ast);
static void print_pipe(struct ast *ast);
static void print_prefix(struct ast *ast);
static void print_element(struct ast *ast);
static void print_sh_cmd(struct ast *ast);
static void print_sp_cmd(struct ast *ast);



static void print_tab(int tab)
{
    for (int t = 0; t < tab; t++)
        printf("    ");
}

void pretty_print(struct ast *tree, int tab)
{
    print_tab(tab);
    if (!tree)
        printf("NULL");
    else if (tree->type == AST_IF)
    {
        printf("if (");
        pretty_print(tree->data->ast_if->condition, tab + 1);
        printf("); then\n");
        pretty_print(tree->data->ast_if->then, tab + 1);
        printf("\n");
        print_tab(tab);
        printf("else\n");
        pretty_print(tree->data->ast_if->else_body, tab + 1);
        printf("\n");
        print_tab(tab);
        printf("fi;\n");
    }
    else if (tree->type == AST_CMD)
    {
        printf("commande [");
        vector_print(tree->data->ast_cmd->arg);
        printf("]");
    }
    else if (tree->type == AST_LIST)
    {
        printf("list \n");
        print_tab(tab);
        printf("{\n");
        pretty_print(tree->data->ast_list->cmd_if[0], tab + 1);
        for (size_t i = 1; i < tree->data->ast_list->size; i++)
        {
            printf(";\n");
            pretty_print(tree->data->ast_list->cmd_if[i], tab + 1);
        }
        printf("\n");
        print_tab(tab);
        printf("}");
    }
    // ADD NEW AST PRINT HERE
}

void ugly_print(struct ast *ast)
{
    if (!ast)
        printf("NULL");
    if (ast->type == AST_IF)
        print_if(ast);
    else if (ast->type == AST_WHILE)
        print_while(ast);
    else if (ast->type == AST_FOR)
        print_for(ast);
    else if(ast->type == AST_UNTIL)
        print_until(ast);
    else if (ast->type == AST_LIST)
        print_list(ast);
    else if (ast->type == AST_CMD)
        print_cmd(ast);
    /*else if (ast->type == AST_PREFIX)
        print_prefix(ast);*/
    else if (ast->type == AST_REDIR)
        print_redirect(ast);
    /*else if (ast->type == AST_SP_CMD)
        print_sp_cmd(ast);
    else if (ast->type == AST_SH_CMD)
        print_sh_cmd(ast);
    else if (ast->type == AST_ELEMENT)
        print_element(ast);*/
    else if (ast->type == AST_AND)
        print_and(ast);
    else if (ast->type == AST_OR)
        print_or(ast);
    else if (ast->type == AST_PIPE)
        print_pipe(ast);
    else if (ast->type == AST_NOT)
        print_not(ast);
}

static void print_if(struct ast *ast)
{
    printf("if ");
    ugly_print(ast->data->ast_if->condition);
    printf("then ");
    ugly_print(ast->data->ast_if->then);
    printf("else ");
    ugly_print(ast->data->ast_if->else_body);
    printf("fi ");
}

static void print_list(struct ast *ast)
{
    ugly_print(ast->data->ast_list->cmd_if[0]);
    for (size_t i = 1; i < ast->data->ast_list->size; i++)
    {
        printf("; ");
        ugly_print(ast->data->ast_list->cmd_if[i]);
    }
}

static void print_cmd(struct ast *ast)
{
    struct vector *v = ast->data->ast_cmd->arg;
    if (v->size == 0)
        return;
    if (v->size == 1)
    {
        printf("%s ", v->data[0]);
        return;
    }
    for (size_t i = 0; i < v->size - 1; i++)
        printf("%s ", v->data[i]);
    print_list(ast->data->ast_cmd->redir);
}

static void print_for(struct ast *ast)
{
    printf("for %s in ", ast->data->ast_for->var);
    for (size_t i = 0; i < ast->data->ast_for->arg->size; i++)
        printf("%s ", ast->data->ast_for->arg->data[i]);
    printf(": do ");
    ugly_print(ast->data->ast_for->for_list);    
}

static void print_while(struct ast *ast)
{
    printf("while ");
    ugly_print(ast->data->ast_while->condition);
    printf("do ");
    ugly_print(ast->data->ast_while->while_body);
}

static void print_until(struct ast *ast)
{
    printf("until ");
    ugly_print(ast->data->ast_until->condition);
    printf("do ");
    ugly_print(ast->data->ast_until->until_body);
}

static void print_and(struct ast *ast)
{
    printf("( ");
    ugly_print(ast->data->ast_and->left);
    printf("&& ");
    ugly_print(ast->data->ast_and->right);
    printf(") ");
}

static void print_or(struct ast *ast)
{
    printf("( ");
    ugly_print(ast->data->ast_or->left);
    printf("|| ");
    ugly_print(ast->data->ast_or->right);
    printf(") ");
}

static void print_not(struct ast *ast)
{
    printf("!");
    ugly_print(ast->data->ast_not->node);
}

/*
static void print_sp_cmd(struct ast *ast)
{
    printf("sp_cmd : \n");
    printf("size_prefix : %lu | ", ast->data->ast_sp_cmd->size_prefix);
    printf("size_element : %lu | ", ast->data->ast_sp_cmd->size_element);
    for (size_t i = 0; i < ast->data->ast_sp_cmd->size_prefix; i++)
    {
        printf("\n");
        ugly_print(ast->data->ast_sp_cmd->cmd_prefix[i]);
    }
    if (ast->data->ast_sp_cmd->word)
        printf("\n%s ", ast->data->ast_sp_cmd->word);
    for (size_t i = 0; i < ast->data->ast_sp_cmd->size_element; i++)
    {
        printf("\n");
        ugly_print(ast->data->ast_sp_cmd->cmd_element[i]);
    }
}

static void print_sh_cmd(struct ast *ast)
{
    printf("sh_cmd ");
    printf("size_redir : %lu ", ast->data->ast_sh_cmd->size_redir);
    if (ast->data->ast_sh_cmd->cmd)
    {
        printf("command ");
        ugly_print(ast->data->ast_sh_cmd->cmd);
        printf("\n");
    }
    for (size_t i = 0; i < ast->data->ast_sh_cmd->size_redir; i++)
    {
        ugly_print(ast->data->ast_sh_cmd->redir[i]);
        printf("\n");
    }
}


static void print_element(struct ast *ast)
{
    printf("element :");
    if (ast->data->ast_element->word)
        printf("%s ", ast->data->ast_element->word);
    if (ast->data->ast_element->redir)
        ugly_print(ast->data->ast_element->redir);
}

static void print_prefix(struct ast *ast)
{
    printf("prefix ");
    if (ast->data->ast_prefix->assign_word)
        printf("%s ", ast->data->ast_prefix->assign_word);
    if (ast->data->ast_prefix->redir)
        ugly_print(ast->data->ast_prefix->redir);
}*/

static void print_redirect(struct ast *ast)
{
    if (ast->data->ast_redir->io_number)
        printf("%s ", ast->data->ast_redir->io_number); 
    printf("redirection ");
    if (ast->data->ast_redir->exit_file)
        printf("%s ", ast->data->ast_redir->exit_file);
}

static void print_pipe(struct ast *ast)
{
    printf("pipe ");
    ugly_print(ast->data->ast_pipe->left);
    printf("| ");
    ugly_print(ast->data->ast_pipe->right);
}

// ADD NEW AST PRINT HERE
