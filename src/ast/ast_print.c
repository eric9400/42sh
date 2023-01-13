#include "ast_print.h"

#include <stdio.h>
#include <stdlib.h>

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
#include "ast_init.h"
#include "ast_print.h"
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

void ugly_print(struct ast *tree)
{
    if (!tree)
        printf("NULL");
    else if (tree->type == AST_IF)
    {
        printf("if ");
        ugly_print(tree->data->ast_if->condition);
        printf("then ");
        ugly_print(tree->data->ast_if->then);
        printf("else ");
        ugly_print(tree->data->ast_if->else_body);
        printf("fi ");
    }
    else if (tree->type == AST_CMD)
    {
        struct vector *v = tree->data->ast_cmd->arg;
        if (v->size == 0)
            return;
        if (v->size == 1)
        {
            printf("%s ", v->data[0]);
            return;
        }
        for (size_t i = 0; i < v->size - 1; i++)
            printf("%s ", v->data[i]);
    }
    else if (tree->type == AST_LIST)
    {
        ugly_print(tree->data->ast_list->cmd_if[0]);
        for (size_t i = 1; i < tree->data->ast_list->size; i++)
        {
            printf("; ");
            ugly_print(tree->data->ast_list->cmd_if[i]);
        }
    }
}
/*
static void print_if(struct ast *ast)
{
    //TODO
}

static void print_list(struct ast *ast)
{
    //TODO
}

static void print_cmd(struct ast *ast)
{
    //TODO
}

static void print_for(struct ast *ast)
{
    //TODO
}

static void print_while(struct ast *ast)
{
    //TODO
}

static void print_until(struct ast *ast)
{
    //TODO
}

static void print_and(struct ast *ast)
{
    //TODO
}

static void print_or(struct ast *ast)
{
    //TODO
}

static void print_not(struct ast *ast)
{
    //TODO
}

static void print_redirect(struct ast *ast)
{
    //TODO
}

static void print_pipe(struct ast *ast)
{
    //TODO
}*/

// ADD NEW AST PRINT HERE