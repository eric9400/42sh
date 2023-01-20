#include "execute_tools.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "ast.h"
#include "expand_tools.h"
#include "hash_map.h"
#include "my_string.h"
#include "vector.h"

static int phoenix_destroyer(struct string *str, struct string *new_str,
                             struct vector *v, int return_value)
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
    phoenix_destroyer(str, new_str, NULL, 0);
}

static int vector_replace(struct vector *vect_temp, struct ast *ast)
{
    if (vect_temp->size == 0)
    {
        vector_destroy(vect_temp);
        return 1;
    }
    else
    {
        vect_temp = vector_append(vect_temp, NULL);
        if (ast->type == AST_CMD)
        {
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
}

static int in_quotes(int *in_s_quotes, int *in_d_quotes, char c)
{
    *in_d_quotes = c == '"';
    *in_s_quotes = c == '\'';
    return *in_d_quotes || *in_s_quotes;
}

static int add_assign_word(char *str, struct string *s, struct string *new_str)
{
    if (str[0] != '#')
        return 0;
    str++;
    char *value = strstr(str, "=");
    if (value != NULL)
    {
        value[0] = '\0';
        value++;
        hash_map_insert(hashmap, str, value);
        destroy_string(s);
        destroy_string(new_str);
        return 1;
    }
    return 0;
}

static size_t size_according_ast(struct ast *ast)
{
    if (ast->type == AST_CMD)
        return ast->data->ast_cmd->arg->size - 1;
    // ast_for vector is not NULL terminated
    return ast->data->ast_for->arg->size;
}

// 39 lines
int expandinho_phoenix(struct ast *ast, int return_value)
{
    // size_t size = ast->data->ast_cmd->arg->size - 1;
    size_t size = size_according_ast(ast);
    struct vector *vect_temp = vector_init(10);
    char buf[2] = { 0 };
    for (size_t i = 0; i < size; i++)
    {
        struct string *str = init_string(ast, i, vect_temp);
        struct string *new_str = init_string(ast, i, vect_temp);
        int in_s_quotes = 0;
        int in_d_quotes = 0;
        if (add_assign_word(str->str, str, new_str))
            continue;
        for (; str->index < str->len; str->index++)
        {
            buf[0] = str->str[str->index];
            // 2.2.2 single quotes
            if (in_s_quotes)
            {
                if (buf[0] == '\'')
                    in_s_quotes = 0;
                else
                    string_append(new_str, buf);
            }
            // 2.2.3 double quotes
            else if (in_d_quotes)
            {
                if (buf[0] == '"')
                    in_d_quotes = 0;
                else if (buf[0] == '$')
                {
                    if (dollar_expansion(str, new_str, return_value, 1))
                        // error case
                        return phoenix_destroyer(str, new_str, vect_temp, 1);
                }
                else if (buf[0] == '\\')
                    // there is always something after a backslash
                    slash_expansion_in_d_quotes(str, new_str, in_d_quotes);
                else
                    string_append(new_str, buf);
            }
            // other char
            else
            {
                if (in_quotes(&in_s_quotes, &in_d_quotes, buf[0]))
                    continue;
                if (buf[0] == '$')
                {
                    if (dollar_expansion(str, new_str, return_value, 0))
                        // error case
                        return phoenix_destroyer(str, new_str, vect_temp, 1);
                }
                else if (buf[0] == '\\')
                    // there is always something after a backslash
                    slash_expansion_in_d_quotes(str, new_str, in_d_quotes);
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

// 37 lines
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
        {
            if (buf[0] == '\'')
                in_s_quotes = 0;
            else
                string_append(new_str, buf);
        }
        // 2.2.3 double quotes
        else if (in_d_quotes)
        {
            if (buf[0] == '"')
                in_d_quotes = 0;
            else if (buf[0] == '$')
            {
                if (dollar_expansion(str, new_str, return_value, in_d_quotes))
                { // error case
                    phoenix_destroyer(str, new_str, NULL, 1);
                    return NULL;
                }
            }
            else if (buf[0] == '\\')
                // there is always something after a backslash
                slash_expansion_in_d_quotes(str, new_str, in_d_quotes);
            else
                string_append(new_str, buf);
        }
        // other char
        else
        {
            if (in_quotes(&in_s_quotes, &in_d_quotes, buf[0]))
                continue;
            if (buf[0] == '$')
            {
                if (dollar_expansion(str, new_str, return_value, in_d_quotes))
                { // error case
                    phoenix_destroyer(str, new_str, NULL, 1);
                    return NULL;
                }
            }
            else if (buf[0] == '\\')
                // there is always something after a backslash
                slash_expansion_in_d_quotes(str, new_str, in_d_quotes);
            else
                string_append(new_str, buf);
        }
    }

    char *return_str = expandinho_junior_2(new_str);
    phoenix_destroyer(str, new_str, NULL, 1);
    return return_str;
}
