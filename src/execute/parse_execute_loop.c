#include "parse_execute_loop.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "ast.h"
#include "builtin.h"
#include "execute.h"
#include "hash_map.h"
#include "lexer.h"
#include "parser.h"
#include "utils.h"

struct toFree *tofree = NULL;

int freeAll(int error)
{
    free_lexer(tofree->lex);
    free_node(tofree->ast);
    fclose(tofree->file);
    if (!is_in_dot)
    {
        hash_map_free(hashM->hashmap);
        f_hash_map_free(hashM->fhashmap);
        free(hashM);
        free(tofree->global_flags);
        int i = 0;
        while (tofree->env_variables[i] != NULL)
        {
            free(tofree->env_variables[i]);
            i++;
        }
        free(tofree->env_variables);
        free(tofree);
    }
    return error;
}

void hash_map_init_basic(void)
{
    if (!is_in_dot)
    {
        char pwd[1000];
        getcwd(pwd, sizeof(pwd));
        char *old = getenv("OLDPWD");
        hash_map_insert(hashM->hashmap, "PWD", pwd);
        if (old != NULL)
            hash_map_insert(hashM->hashmap, "OLDPWD", old);
        else
            hash_map_insert(hashM->hashmap, "OLDPWD", pwd);
        hash_map_insert(hashM->hashmap, "IFS", " \t\n");
    }
}

// 36 lines
int parse_execute_loop(FILE *f, struct flags *flags)
{
    if (!is_in_dot)
        tofree = calloc(1, sizeof(struct toFree));
    tofree->global_flags = flags;
    tofree->lex = init_lexer(f);
    tofree->ast = NULL;
    tofree->file = f;
    if (!is_in_dot)
        tofree->env_variables = calloc(20, sizeof(char *));
    int return_value = 0;
    hash_map_init_basic();
    // RAJOUTER UN ETAT D'AST POUR QUAND
    while (1)
    {
        tofree->ast = input(tofree->lex);
        // case when error during parsing OR eof
        if (tofree->lex->error || (tofree->lex->error == 0 && !tofree->ast))
            break;
        if (tofree->lex->error)
        {
            if (tofree->file != stdin)
                return freeAll(tofree->lex->error);
        }
        else if (!tofree->ast && tofree->file != stdin)
            break;
        else
        {
            // pretty print
            if (flags->p)
            {
                ugly_print(tofree->ast, 0);
                printf("\n");
            }

            // ugly print && exit
            if (flags->u)
            {
                ugly_print(tofree->ast, 0);
                printf("\n");
                break;
            }
            // if (ast != SPECIFIC_AST_FOR_END_OF_LINE (FOR EXAMPLE))
            if (tofree->ast->type == AST_CMD
                && tofree->ast->data->ast_cmd->arg->data[0][0] == '\0')
            {
                free_node(tofree->ast);
                continue;
            }
			fflush(tofree->file);
            return_value = execute(tofree->ast, return_value);
			if (return_value)
            {
                if (tofree->file != stdin)
                    return freeAll(return_value);
            }
        }
        free_node(tofree->ast);
        fflush(stdout);
    }
    if (tofree->lex->error)
        return freeAll(tofree->lex->error);
    return freeAll(return_value);
}
