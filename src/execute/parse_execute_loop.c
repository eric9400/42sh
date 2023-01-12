#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ast.h"
#include "execute.h"
#include "lexer.h"
#include "parser.h"
#include "utils.h"
#include "hash_map.h"

static struct flags *global_flags = NULL;
struct hash_map *hashmap = NULL;

static int freeAll(FILE *file, struct lexer *lex, struct ast *ast, int error)
{
    free_lexer(lex);
    free_node(ast);
    fclose(file);
    free(global_flags);
    hash_map_free(hashmap);
    return error;
}

int parse_execute_loop(FILE *file, struct flags *flags)
{
    global_flags = flags;
    struct lexer *lex = init_lexer(file);
    struct ast *ast = NULL;
    int return_value = 0;
    hashmap = hash_map_init(20);
    hash_map_insert(hashmap, "a", "echo bonjour");
    // hash_map_insert_basics(hashmap);
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
                pretty_print(ast, 0);

            // ugly print && exit
            if (flags->u)
            {
                ugly_print(ast);
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
            return_value = execute(
                ast, return_value); // RETURN_VALUE FOR ECHO EXPAND (echo $?)
            if (return_value)
            {
                /*
                if (file == stdin)
                    fprintf(stderr, "Execute error TO COMPLETE\n");
                else
                    return freeAll(file, lex, ast, lex->error);
                */
                if (file != stdin)
                    return freeAll(file, lex, ast, lex->error);
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
