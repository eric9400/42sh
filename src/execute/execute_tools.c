#include "execute_tools.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#include "ast.h"
#include "expand_tools.h"
#include "hash_map.h"
#include "my_string.h"
#include "vector.h"

char *is_special_var(char *str, int return_value)
{
    char buf[1000];

    /*if (!strcmp(str, "@") || !strcmp(str, "*")){}else */
    if (atoi(str) != 0 || !strcmp(str, "#") || !strcmp(str, "OLDPWD") || !strcmp(str, "PWD")
        || !strcmp(str, "IFS"))
    {
        const char *res = hash_map_get(hashmap, str);
        if (!res)
            return NULL;
        return strdup(res);
    }
    else if (!strcmp(str, "?"))
    {
        sprintf(buf, "%d", return_value);
        return strdup(buf);
    }
    else if (!strcmp(str, "$"))
    {
        sprintf(buf, "%d", getpid());
        return strdup(buf);
    }
    else if (!strcmp(str, "RANDOM"))
    {
        srand(time(NULL));
        // max random possible
        int temp = rand() % 32768;
        sprintf(buf, "%d", temp);
        return strdup(buf);
    }
    else if (!strcmp(str, "UID"))
    {
        uid_t uid = getuid();
        sprintf(buf, "%d", uid);
        return strdup(buf);
    }
    return NULL;
}

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

int expandinho_phoenix(struct ast *ast)
{
    size_t size = ast->data->ast_cmd->arg->size - 1;
    struct vector *vect_temp = vector_init(10);
    int in_s_quotes = 0;
    int in_d_quotes = 0;
    char buf[2] = { 0 };
    struct string *str = NULL;
    struct string *new_str = NULL;
    for (size_t i = 0; i < size; i++)
    {
        str = init_string(ast->data->ast_cmd->arg->data[i], 0, strlen(ast->data->ast_cmd->arg->data[i]));
        new_str = init_string(str->str, 0, str->len);
        in_s_quotes = 0;
        in_d_quotes = 0;
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
                    if (dollar_expansion(str, new_str))
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
                    if (dollar_expansion(str, new_str))
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
        new_str->str = realloc(new_str->str, new_str->index + 1);
        new_str->str[new_str->index] = '\0';
        if (new_str->index != 0)
            vect_temp = vector_append(vect_temp, strdup(new_str->str));
        expandinho_phoenix_destroyer(str, new_str, NULL, 0);
    }
    vect_temp = vector_append(vect_temp, NULL);
    vector_destroy(ast->data->ast_cmd->arg);
    ast->data->ast_cmd->arg = vect_temp;
    return 0;
}

char *expandinho_phoenix_junior(char *s)
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
                if (dollar_expansion(str, new_str))
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
                if (dollar_expansion(str, new_str))
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
    new_str->str = realloc(new_str->str, new_str->index + 1);
    new_str->str[new_str->index] = '\0';
    char *return_str = strdup(new_str->str);
    expandinho_phoenix_destroyer(str, new_str, NULL, 1);
    return return_str;
}
