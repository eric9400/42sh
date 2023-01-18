#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "ast.h"
#include "execute.h"
#include "lexer.h"
#include "parser.h"
#include "utils.h"
#include "hash_map.h"

struct flags *global_flags = NULL;
struct lexer *lex = NULL;
struct ast *ast = NULL;
FILE *file = NULL;

int freeAll(FILE *file, struct lexer *lex, struct ast *ast, int error)
{
    free_lexer(lex);
    free_node(ast);
    fclose(file);
    free(global_flags);
    hash_map_free(hashmap);
    return error;
}

void hash_map_init_basic(void)
{
    char pwd[1000];
    getcwd(pwd, sizeof(pwd));
    hash_map_insert(hashmap, "PWD", pwd);
    hash_map_insert(hashmap, "OLDPWD", pwd);
    hash_map_insert(hashmap, "IFS", " \t\n");
}

int parse_execute_loop(FILE *file, struct flags *flags)
{
    global_flags = flags;
    lex = init_lexer(file);
    ast = NULL;
    file = file;
    int return_value = 0;
    hash_map_init_basic();
    /*
    if (file == stdin)
        printf("42sh$ ");
    */
    // RAJOUTER UN ETAT D'AST POUR QUAND
    while (1)
    {
        ast = input(lex);
        if (lex->error == 0 && !ast)
            break;
        if (lex->error)
        {
            /*
            if (file == stdin)
                fprintf(stderr, "Parsing error TO COMPLETE\n");
            else
                return freeAll(file, lex, ast, lex->error);
            */
            if (file != stdin)
                return freeAll(file, lex, ast, lex->error);
        }
        else if (!ast && file != stdin)
            break;
        else
        {
            // pretty print
            if (flags->p)
            {
                ugly_print(ast, 0);
                printf("\n");
            }

            // ugly print && exit
            if (flags->u)
            {
                ugly_print(ast, 0);
                printf("\n");
                break;
            }
            // if (ast != SPECIFIC_AST_FOR_END_OF_LINE (FOR EXAMPLE))
            if (ast->type == AST_CMD
                && ast->data->ast_cmd->arg->data[0][0] == '\0')
            {
                free_node(ast);
                continue;
            }
            return_value = execute(ast, return_value);
            if (return_value)
            {
                /*
                if (file == stdin)
                    fprintf(stderr, "Execute error TO COMPLETE\n");
                else
                    return freeAll(file, lex, ast, lex->error);
                */
                if (file != stdin)
                    return freeAll(file, lex, ast, return_value);
            }
        }
        free_node(ast);
        /*
        if (file == stdin)
            printf("42sh$ ");
        */
        fflush(stdout);
    }
    if (lex->error)
        return freeAll(file, lex, ast, lex->error);
    return freeAll(file, lex, ast, return_value);
}
