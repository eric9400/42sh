#include "execute_tools.h"
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hash_map.h"

void split_vector(int *marker, struct ast *ast)
{
    size_t len = ast->data->ast_cmd->arg->size;
    struct vector *v = vector_init(len + 1);
    char **data = ast->data->ast_cmd->arg->data;
    for (size_t i = 0; i < len - 1; i++) ///BABABJIIIIIIIIIIIIIIi
    {
        // case when we have assignment word
        if (marker[i] == -1)
            continue;
        // case when we expanded
        if (marker[i])
        {
            char *temp = strdup(data[i]);
            split_and_append(&v, temp);
        }
        else
            v = vector_append(v, strdup(data[i]));
    }
    v = vector_append(v, NULL);
    vector_destroy(ast->data->ast_cmd->arg);
    ast->data->ast_cmd->arg = v;
}

int is_char_variable(char c)
{
    return  (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_';
}

// return "$@"
void split_quote_at(struct vector **v)
{
    int count = 1;
    while (1)
    {
        char key[1000];
        sprintf(key, "%d", count);
        const char *value = hash_map_get(hashmap, key);
        if (!value)
            return;
        *v = vector_append(*v, strdup(value));
        count++;
    }
}

void split_and_append(struct vector **v, char *s)
{
    size_t i = 0;
    size_t start = 0;
    while (s[i] != '\0')
    {
        if (s[i] == ' ' && i - start > 0)
        {
            char *copy = strndup(s + start, i - start);
            *v = vector_append(*v, copy);
            start = i + 1;
        }
        else if (s[i] == ' ')
            start++;
        i++;
    }
    if (i - start > 0)
        *v = vector_append(*v, strndup(s + start, i - start));
    free(s);
}

// return $* or $@
void split_no_quote(struct vector **v)
{
    int count = 1;
    while (1)
    {
        char key[1000];
        sprintf(key, "%d", count);
        const char *value = hash_map_get(hashmap, key);
        if (!value)
            return;
        split_and_append(v, strdup(value));
        count++;
    }
}

/*
 * \return 0 if expand name is valid else return 1
 * lenvar will contain the size of the expand var name + 1 ('$')
 */
static int is_still_variable(char *str, int *lenvar)
{
    // detecting special var
    if (str[0] != '\0'
        && ((str[0] == '@') || (str[0] == '?') || (str[0] == '$')
            || (str[0] == '#') || (str[0] == '*'))
        && str[1] == '\0')
    {
        *lenvar = 2;
        return 0;
    }

    int i = 0;
    int res = 0;
    while (str[i] != '\0' && !isspace(str[i]) && str[i] != '$')
    {
        if (is_char_variable(str[i]))
            i++;
        else
        {
            res = 1;
            break;
        }
    }
    // return when no problem
    *lenvar = i + 1;
    return res;
}

// return "$*"
void split_quote_star(struct vector **v)
{
    int count = 1;
    char *str = malloc(1);
    size_t len_str = 1;
    while (1)
    {
        if (len_str != 1)
            str = strcat(str, " ");
        char key[1000];
        sprintf(key, "%d", count);
        const char *value = hash_map_get(hashmap, key);
        if (!value)
            return;
        len_str += strlen(value) + 1;
        str = realloc(str, sizeof(char) * len_str);
        str = strcat(str, strdup(value));
        count++;
    }
    str[len_str - 1] = '\0';
    *v = vector_append(*v, str);
}

static char *is_special_var(char *str, int return_value)
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

static int is_valid_char(char c)
{
    if (c >= 'a' && c <= 'z')
        return 1;
    else if (c >= 'A' && c <= 'Z')
        return 1;
    else if (c >= '0' && c <= '9')
        return 1;
    return c == '_';
}

static void resize_string(char **str, size_t size, size_t *capacity)
{
    if (size < *capacity)
        return;
    *capacity = size * 2;
    *str = realloc(*str, *capacity);
}

static string_append(struct string *str, struct string *new_str, char *buf)
{
    resize_string(&new_str->str, new_str->index + strlen(buf) + 1, new_str->len);
    new_str->str = strcat(new_str->str, buf);
    new_str->index += strlen(buf);
}

static void expand_from_hashmap(struct string *str, struct string *new_str, char *buf)
{
    char *value = hashmap_get_copy(hashmap, buf);
    string_append(str, new_str, value); 
}

static int dollar_expansion(struct string *str, struct string *new_str)
{
    str->index += 1;
    char buf[5] = { 0 };
    // ${a}
    // need to handle error cases "echo ${+}" for example
    if (str->str[str->index] == '{')
    {
        size_t start = str->index;
        while (is_valid_char(str->str[str->index]))
            str->index += 1;
        if (str->str[str->index] == '}')
        {
            char *to_append = strndup(str->str[start], str->index - start);
            string_append(str, new_str, to_append);
            free(to_append);
            return 0;
        }
        // if no matching '}' => error
        return 1;
    }
    // $a
    else if (is_valid_char(str->str[str->index]))
    {
        if (isdigit(str->str[str->index]))
        {
            buf[0] = str->str[str->index];
            expand_from_hashmap(str, new_str, buf);
        }

        size_t start = str->index;
        while(is_valid_char(str->str[str->index]))
            str->index += 1;
        char *to_append = strndup(str->str[start], str->index - start);
        string_append(str, new_str, to_append);
        free(to_append);
    }
    else
    {
        // $_ with invalid char after $, just print $_
        buf[0] = '$';
        buf[1] = str->str[str->index];
        string_append(str, new_str, buf);
        str->index -= 1;
    }
    return 0;
}

static struct string *init_string(char *str, size_t index, size_t len)
{
    struct string *obj = malloc(sizeof(struct string));
    obj->str = strdup(str);
    obj->index = index;
    obj->len = len;
    return obj;
}

static void destroy_string(struct string *str)
{
    free(str->str);
    free(str);
}

static void expandhino_phoenix_destroyer(struct string *str, struct string *new_str, struct vector *v)
{
    if (v)
        vector_destroy(v);
    if (str)
        destroy(str);
    if (new_str)
        destroy(new_str);
}

int expandhino_phoenix(struct ast *ast)
{
    size_t size = ast->data->ast_cmd->arg->size - 1;
    struct vector *vect_temp = vector_init(10);
    int in_s_quotes = 0;
    int in_d_quotes = 0;
    int was_slash = 0;
    size_t start = 0;
    char buf[2] = { 0 };
    struct string *str = NULL;
    struct string *new_str = NULL;
    for (size_t i = 0; i < size; i++)
    {
        str = init_string(ast->data->ast_cmd->arg->data[i], 0, strlen(str));
        new_str = init_string(str->str, 0, strlen(str));
        in_s_quotes = 0;
        in_d_quotes = 0;
        was_slash = 0;
        for (; str->index < str->len; str->index++)
        {
            buf[0] = str->str[str->index];
            if (in_s_quotes)
            {
                if (str->str[str->index] == ''')
                {
                    in_s_quotes = 0;
                    continue;
                }
                else
                    string_append(str, new_str, buf);
            }
            else if (str->str[str->index] == '$')
            {
                if (dollar_expansion(str, new_str))
                {
                    // error case
                    expandinho_phoenix_destroyer(str, new_str, vect_temp);
                    return 1;
                }
            }
            else if (str->str[str->index] == '"')
            {
                
            }
            else if (str->str[str->index] == '\\')
            {
                if (was_slash)
                    string_append(&new_str, curr, &capacity, buf);
                else
                {
                    was_slash = 1;
                    continue;
                }
            }
            else
                string_append(&new_str, curr, &capacity, buf);
            was_slash = 0;
            //new_curr++;
        }
        expandinho_phoenix_destroyer(str, new_str, NULL);
        new_str = realloc(new_str, new_curr + 1);
        new_str[new_curr] = '\0';
    }
    vect_temp = vector_append(vect_temp, NULL);
    vector_destroy(ast->data->ast_cmd->arg);
    ast->data->ast_cmd->arg = vect_temp;
    return 0;
}
