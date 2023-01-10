#include <stdio.h>
#include <stdlib.h>

#include "ast.h"
#include "utils.h"
#include "lexer.h"
#include "parser.h"
#include "execute.h"

int freeAll(FILE *file, struct lexer *lex, struct ast *ast, int error)
{
    free_lexer(lex);
    free_node(ast);
    fclose(file);
    return error;
}

int parse_execute_loop(FILE *file, struct flags *flags)
{
    struct lexer *lex = init_lexer(file);
    struct ast *ast = NULL;
    int p = flags->p;
    free(flags);
    int return_value = 0;
    while ((ast = input(lex)) != NULL || file == stdin) // RAJOUTER UN ETAT D'AST POUR QUAND 
    {
        if (lex->error)
        {
            if (file == stdin)
                fprintf(stderr, "Parsing error TO COMPLETE\n");
            else
                return freeAll(file, lex, ast, lex->error);
        }
        else
        {
            if (p)
                pretty_print(ast, 0);
            // if (ast != SPECIFIC_AST_FOR_END_OF_LINE (FOR EXAMPLE))
            return_value = execute(ast, return_value); //RETURN_VALUE FOR ECHO EXPAND (echo $?)
            if (return_value)
            {
                if (file == stdin)
                    fprintf(stderr, "Execute error TO COMPLETE\n");
                else
                    return freeAll(file, lex, ast, lex->error);
            }
        }
    }
    if (lex->error)
        return freeAll(file, lex, ast, lex->error);
    return freeAll(file, lex, ast, return_value);
}
