#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "execute.h"
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

int main(int argc, char *argv[])
{        
    char *filename = NULL;
    int opt = -1;
    FILE *file = NULL;

    // parsing arguments
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

    // lexing && parsing
    struct lexer *lex = init_lexer(file);
    struct ast *ast = input(lex);
    /*while (1)
    {
        if (ast)
        {
            execute(ast);
            free_node(ast);
        }
        ast = input(lex);
    }*/

    // pretty print
    if (lex->error == 0 && opt)
    {
        //pretty_print(ast, 0);
        ugly_print(ast);
        printf("\n");
    }
    else if (lex->error != 0)
    {
        printf("ERROR DANGER MAYDAY MAYDAY PARSOR\n");
        exit(2);
    }
    //int error = lex->error;
    free_lexer(lex);

    //int value = 0;
    // execute
    //if (ast)
    //    value = execute(ast);

    if (ast)
        free_node(ast);

    fflush(stdout);
    return 0;
}
