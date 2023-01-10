#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "ast.h"
#include "utils.h"

/*
 * \brief Parsing of inputs
 * \details auto parsing
 * \param argc argv from main, filename *.sh, d_opt to know which flags set
 * \return 0 if no args, 0 if -c, 1 if -p, 2 if both
 */
int BaBaJi(int argc, char **argv, char **filename, struct flags *flags)
{
    if (argc == 1)
        return 0;

    int opt;
    while ((opt = getopt(argc, argv, ":cp")) != -1)
    {
        switch (opt)
        {
            case 'c':
                flags->c = 1;
                break;
            case 'p':
                flags->p = 1; 
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
    else if ((argc == 2 && flags->p) || argc == 1)
        return 0;
    else
        errx(2,"Usage : 42sh [OPTIONS] [SCRIPT] [ARGUMENTS ...]\n");
}

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