#include "execute_tools.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "hash_map.h"

void split_vector(int *marker, struct ast *ast)
{
    size_t len = ast->data->ast_cmd->arg->size;
    struct vector *v = vector_init(len + 1);
    char **data = ast->data->ast_cmd->arg->data;
    for (size_t i = 0; i < len - 1; i++)
    {
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
