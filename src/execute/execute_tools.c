#include "execute_tools.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "ast.h"
#include "builtin.h"
#include "execute.h"
#include "expand_tools.h"
#include "hash_map.h"
#include "hash_map_global.h"
#include "my_string.h"
#include "redirection.h"
#include "vector.h"

static int in_s_quotes = 0;
static int in_d_quotes = 0;
// check if we found a single or double quote at least once
static int quotes = 0;
static int return_value = 0;

static int phoenix_destroyer(struct string *str, struct string *new_str,
                             struct vector *v)
{
    if (v)
        vector_destroy(v);
    if (str)
        destroy_string(str);
    if (new_str)
        destroy_string(new_str);
    return return_value;
}

static void expandinho_phoenix_2(struct string *str, struct string *new_str,
                                 struct vector **vect_temp)
{
    new_str->str = realloc(new_str->str, new_str->index + 1);
    new_str->str[new_str->index] = '\0';
    if (new_str->index != 0)
        *vect_temp = vector_append(*vect_temp, strdup(new_str->str));
    phoenix_destroyer(str, new_str, NULL);
}

static int vector_replace(struct vector *vect_temp, struct ast *ast)
{
    if (vect_temp->size == 0)
    {
        if (!quotes)
        {
            vector_destroy(vect_temp);
            return 1;
        }
        vect_temp = vector_append(vect_temp, strdup(""));
    }
    if (ast->type == AST_CMD)
    {
        vect_temp = vector_append(vect_temp, NULL);
        vector_destroy(ast->data->ast_cmd->arg);
        ast->data->ast_cmd->arg = vect_temp;
    }
    else
    {
        vector_destroy(ast->data->ast_for->arg);
        ast->data->ast_for->arg = vect_temp;
    }
    return 0;
}

static int in_quotes(char c)
{
    in_d_quotes = c == '"';
    in_s_quotes = c == '\'';
    quotes = quotes || in_d_quotes || in_s_quotes;
    return in_d_quotes || in_s_quotes;
}

// 37 lines
static int add_assign_word(struct ast *ast, char *str, struct string *s,
                           struct string *new_str)
{
    char *temp = NULL;

    // edge case for export
    if (ast->type == AST_CMD
        && (!strcmp(ast->data->ast_cmd->arg->data[0], "export")
            || !strcmp(ast->data->ast_cmd->arg->data[0], "alias")))
    {
        if (str[0] == '#')
        {
            temp = strdup(ast->data->ast_cmd->arg->data[1] + 1);
            free(ast->data->ast_cmd->arg->data[1]);
            ast->data->ast_cmd->arg->data[1] = temp;
            s->index++;
        }
        return 0;
    }
    if (str[0] != '#')
        return 0;
    str++;
    char *value = strstr(str, "=");

    // supposed to be always true if parser working
    if (value != NULL)
    {
        value[0] = '\0';
        value++;
        int is_s_quotes = 0;

        // a="$1"
        // remove single quotes and double quotes from value
        if (value[0] == '"' || value[0] == '\'')
        {
            is_s_quotes = value[0] == '\'';
            char buf[2] = { 0 };
            buf[0] = value[0];
            value++;
            temp = strstr(value, buf);
            if (temp)
                temp[0] = '\0';
        }

        // a=$1
        int need_to_free = 0;

        // expand if not in single quotes
        if (!is_s_quotes && value[0] == '$')
        {
            need_to_free = 1;
            value = expandinho_phoenix_junior(value, return_value);
        }

        // a=b
        hash_map_insert(hashM->hashmap, str, value);
        if (need_to_free)
            free(value);
        destroy_string(s);
        destroy_string(new_str);
        return 1;
    }
    return 0;
}

static size_t size_according_ast(struct ast *ast, int ret_value)
{
    // init quotes value
    return_value = ret_value;
    quotes = 0;
    if (ast->type == AST_CMD)
        return ast->data->ast_cmd->arg->size - 1;
    // ast_for vector is not NULL terminated
    return ast->data->ast_for->arg->size;
}

void s_quotes_cond(char buf[2], struct string *new_str)
{
    if (buf[0] == '\'')
        in_s_quotes = 0;
    else
        string_append(new_str, buf);
}

// 40 lines
int expandinho_phoenix(struct ast *ast, int ret_value)
{
    size_t size = size_according_ast(ast, ret_value);
    struct vector *vect_temp = vector_init(10);
    char buf[2] = { 0 };
    for (size_t i = 0; i < size; i++)
    {
        struct string *str = init_string(ast, i, vect_temp);
        struct string *new_str = init_string(ast, i, vect_temp);
        in_s_quotes = 0;
        in_d_quotes = 0;
        if (add_assign_word(ast, str->str, str, new_str))
            continue;
        for (; str->index < str->len; str->index++)
        {
            buf[0] = str->str[str->index];
            // 2.2.2 single quotes
            if (in_s_quotes)
                s_quotes_cond(buf, new_str);
            // 2.2.3 double quotes
            else if (in_d_quotes)
            {
                if (buf[0] == '"')
                    in_d_quotes = 0;
                else if (buf[0] == '$')
                {
                    if (dollar_expansion(str, new_str, return_value, 1))
                        // error case
                        return phoenix_destroyer(str, new_str, vect_temp);
                }
                else if (buf[0] == '\\')
                    // there is always something after a backslash
                    slash_expansion_in_d_quotes(str, new_str, in_d_quotes);
                else if (buf[0] == '`')
                    command_substitution(str, new_str, '`');
                else
                    string_append(new_str, buf);
            }
            // other char
            else
            {
                if (in_quotes(buf[0]))
                    continue;
                if (buf[0] == '$')
                {
                    if (dollar_expansion(str, new_str, return_value, 0))
                        // error case
                        return phoenix_destroyer(str, new_str, vect_temp);
                }
                else if (buf[0] == '\\')
                    // there is always something after a backslash
                    slash_expansion_in_d_quotes(str, new_str, in_d_quotes);
                else if (buf[0] == '`')
                    command_substitution(str, new_str, '`');
                else
                    string_append(new_str, buf);
            }
        }
        expandinho_phoenix_2(str, new_str, &vect_temp);
    }
    return vector_replace(vect_temp, ast);
}

static char *expandinho_junior_2(struct string *new_str)
{
    new_str->str = realloc(new_str->str, new_str->index + 1);
    new_str->str[new_str->index] = '\0';
    char *return_str = strdup(new_str->str);
    return return_str;
}

// 38 lines
char *expandinho_phoenix_junior(char *s, int return_value)
{
    int in_s_quotes = 0;
    int in_d_quotes = 0;
    char buf[2] = { 0 };
    struct string *str = init_string2(s, 0, strlen(s));
    struct string *new_str = init_string2(str->str, 0, str->len);
    for (; str->index < str->len; str->index++)
    {
        buf[0] = str->str[str->index];
        // 2.2.2 single quotes
        if (in_s_quotes)
            s_quotes_cond(buf, new_str);
        // 2.2.3 double quotes
        else if (in_d_quotes)
        {
            if (buf[0] == '"')
                in_d_quotes = 0;
            else if (buf[0] == '$')
            {
                if (dollar_expansion(str, new_str, return_value, in_d_quotes))
                { // error case
                    phoenix_destroyer(str, new_str, NULL);
                    return NULL;
                }
            }
            else if (buf[0] == '`')
                command_substitution(str, new_str, '`');
            else if (buf[0] == '\\')
                // there is always something after a backslash
                slash_expansion_in_d_quotes(str, new_str, in_d_quotes);
            else
                string_append(new_str, buf);
        }
        // other char
        else
        {
            if (in_quotes(buf[0]))
                continue;
            if (buf[0] == '$')
            {
                if (dollar_expansion(str, new_str, return_value, in_d_quotes))
                { // error case
                    phoenix_destroyer(str, new_str, NULL);
                    return NULL;
                }
            }
            else if (buf[0] == '`')
                command_substitution(str, new_str, '`');
            else if (buf[0] == '\\')
                // there is always something after a backslash
                slash_expansion_in_d_quotes(str, new_str, in_d_quotes);
            else
                string_append(new_str, buf);
        }
    }

    char *return_str = expandinho_junior_2(new_str);
    phoenix_destroyer(str, new_str, NULL);
    return return_str;
}

void print_hash_map(void)
{
    int size = hashM->hashmap->size;
    for (int i = 0; i < size; i++)
    {
        printf("%d ->", i);
        if (hashM->hashmap->data != NULL)
        {
            struct pair_list *curr = hashM->hashmap->data[i];
            while (curr)
            {
                printf("( %s, %s) ", curr->key, curr->value);
                curr = curr->next;
            }
        }
        printf("\n");
    }
}

// 22 lines
int check_function(char **str, int return_value)
{
    struct ast *ast = f_hash_map_get(hashM->fhashmap, str[0]);
    if (ast == NULL) // if this is not a function in the hash_map
        return -1;

    int error_redir = 0;
    struct stock_fd *stock_fd =
        func_redir(ast->data->ast_cmd->redir, return_value, &error_redir);
    if (stock_fd == NULL && error_redir != 0)
        return error_redir;
    char **old_hashmap = copy_values();
    int i = 1;
    char value[100] = { 0 };
    while (str[i] != NULL)
    {
        sprintf(value, "%d", i);
        hash_map_insert(hashM->hashmap, value, str[i]);
        i++;
    }
    sprintf(value, "%d", i);
    while (hash_map_remove(hashM->hashmap, value))
    {
        i += 1;
        sprintf(value, "%d", i);
    }
    int res = execute(ast->data->ast_func->func, return_value);
    hash_map_restore(old_hashmap);
    return res;
}
