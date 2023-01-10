#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "execute.h"
#include "parser.h"
#include "utils.h"

int main(int argc, char **argv)
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

    // pretty print
    if (lex->error == 0 && opt)
    {
        pretty_print(ast, 0);
        printf("\n");
    }
    else if (lex->error != 0)
    {
        printf("ERROR DANGER MAYDAY MAYDAY PARSOR\n");
        exit(2);
    }

    //int error = lex->error;
    free_lexer(lex);

    int value = 0;
    if (ast)
        value = execute(ast);

    if (ast)
        free_node(ast);

    return value;
}
