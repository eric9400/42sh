#include "my_string.h"

#include <stdlib.h>
#include <string.h>

struct string *init_string(struct ast *ast, int i, struct vector *v)
{
    char *str = NULL;
    if (ast->type == AST_CMD)
        str =  ast->data->ast_cmd->arg->data[i];
    else
        str = ast->data->ast_for->arg->data[i];
    struct string *obj = malloc(sizeof(struct string));
    obj->str = strdup(str);
    obj->index = 0;
    obj->len = strlen(str);
    obj->v = v;
    return obj;
}

struct string *init_string2(char *str, size_t index, size_t len)
{
    struct string *obj = malloc(sizeof(struct string));
    obj->str = strdup(str);
    obj->index = index;
    obj->len = len;
    return obj;
}

struct string *init_string3(size_t len)
{
    struct string *obj = malloc(sizeof(struct string));
    obj->str = calloc(sizeof(len) + 1, 1);
    obj->index = 0;
    obj->len = len;
    return obj;
}

void my_strcat(char *dest, char *src)
{
    size_t i = 0;
    while (src[i])
    {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
}

void destroy_string(struct string *str)
{
    free(str->str);
    free(str);
}
