#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "parser.h"

static int BaBaJi(int argc, char *argv[], char **filename, int *d_opt)
{
    if (argc == 1)
        return 0;

    int opt;
    while ((opt = getopt(argc, argv, ":cp")) != -1)
    {
        switch (opt)
        {
        case 'c':
            if (*d_opt >= 1)
                *d_opt = 2;
            else
                *d_opt = 0; 
            break;
        //pretty-print opt
        case 'p':
            if (*d_opt >= 0)
                *d_opt = 2;
            else   
                *d_opt = 1;
            break;
        case ':':
            errx(2,"Usage : 42sh [OPTIONS] [SCRIPT] [ARGUMENTS ...]\n");
        case '?':
            errx(2,"Usage : 42sh [OPTIONS] [SCRIPT] [ARGUMENTS ...]\n");
        }
    }
    if (argc - optind == 1)
    {
        *filename = strdup(argv[optind]);
        return 0;
    }
    else
        errx(2,"Usage : 42sh [OPTIONS] [SCRIPT] [ARGUMENTS ...]\n");
}

void print_tab(int tab)
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
        pretty_print(tree->data->ast_if->condition, 0);
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
        printf("list \n{\n");
        pretty_print(tree->data->ast_list->cmd_if[0], tab + 1);
        for (size_t i = 1; i < tree->data->ast_list->size; i++)
        {
            printf(";\n");
            pretty_print(tree->data->ast_list->cmd_if[i], tab + 1);
        }
        printf("\n}");
    }
    // ADD NEW AST PRINT HERE
}

int main(int argc, char *argv[])
{        
    char *filename = NULL;
    int opt = -1;
    FILE *file = NULL;

    BaBaJi(argc, argv, &filename, &opt);
    if (!opt || opt == 2)
    {
        file = fopen(filename, "r");
        if (!file)
            return 2;
    }
    else if (filename != NULL)
        file = fmemopen(filename, strlen(filename), "r");
    else
        file = stdin;
    free(filename);

    struct lexer *lex = init_lexer(file);
    struct ast *ast = input(lex);

    if (lex->error == 0)
    {
        pretty_print(ast, 0);
        printf("\n");
    }
    else
        printf("Error: AST IS REFUSED\n");

    if (ast)
        free_node(ast);

    int error = lex->error;
    free_lexer(lex);
    fflush(stdout);
    return error;
}
