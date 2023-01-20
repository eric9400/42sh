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

static int expandinho_phoenix_destroyer(struct string *str, struct string *new_str, struct vector *v, int return_value)
{
    if (v)
        vector_destroy(v);
    if (str)
        destroy_string(str);
    if (new_str)
        destroy_string(new_str);
    return return_value;
}

static void expandinho_phoenix_2(struct string *str, struct string *new_str, struct vector **vect_temp)
{
    new_str->str = realloc(new_str->str, new_str->index + 1);
    new_str->str[new_str->index] = '\0';
    if (new_str->index != 0)
        *vect_temp = vector_append(*vect_temp, strdup(new_str->str));
    expandinho_phoenix_destroyer(str, new_str, NULL, 0);
}

static void vector_replace(struct vector *vect_temp, struct ast *ast)
{
    vect_temp = vector_append(vect_temp, NULL);
    vector_destroy(ast->data->ast_cmd->arg);
    ast->data->ast_cmd->arg = vect_temp;
}

// 40 lines
int expandinho_phoenix(struct ast *ast, int return_value)
{
    size_t size = ast->data->ast_cmd->arg->size - 1;
    struct vector *vect_temp = vector_init(10);
    char buf[2] = { 0 };
    for (size_t i = 0; i < size; i++)
    {
        struct string *str = init_string(ast->data->ast_cmd->arg->data[i], 0, strlen(ast->data->ast_cmd->arg->data[i]));
        struct string *new_str = init_string(str->str, 0, str->len);
        int in_s_quotes = 0;
        int in_d_quotes = 0;
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
                    if (dollar_expansion(str, new_str, return_value))
                        // error case
                        return expandinho_phoenix_destroyer(str, new_str, vect_temp, 1);
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
                in_d_quotes = buf[0] == '"';
                in_s_quotes = buf[0] == '\'';
                if (in_d_quotes || in_s_quotes)
                    continue;
                if (buf[0] == '$')
                {
                    if (dollar_expansion(str, new_str, return_value))
                        // error case
                        return expandinho_phoenix_destroyer(str, new_str, vect_temp, 1);
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
    vector_replace(vect_temp, ast);
    return 0;
}

static char *expandinho_junior_2(struct string *new_str)
{
    new_str->str = realloc(new_str->str, new_str->index + 1);
    new_str->str[new_str->index] = '\0';
    char *return_str = strdup(new_str->str);
    return return_str;
}
// 39 lines
char *expandinho_phoenix_junior(char *s, int return_value)
{
    int in_s_quotes = 0;
    int in_d_quotes = 0;
    char buf[2] = { 0 };
    struct string *str = init_string(s, 0, strlen(s));
    struct string *new_str = init_string(str->str, 0, str->len);
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
                if (dollar_expansion(str, new_str, return_value))
                {    // error case
                    expandinho_phoenix_destroyer(str, new_str, NULL, 1);
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
            in_d_quotes = buf[0] == '"';
            in_s_quotes = buf[0] == '\'';
            if (in_d_quotes || in_s_quotes)
                continue;
            if (buf[0] == '$')
            {
                if (dollar_expansion(str, new_str, return_value))
                {    // error case
                    expandinho_phoenix_destroyer(str, new_str, NULL, 1);
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
    expandinho_phoenix_destroyer(str, new_str, NULL, 1);
    return return_str;
}
