#include "my_string.h"

#include <stdlib.h>
#include <string.h>

struct string *init_string(struct ast *ast, int i)
{
    char *str = ast->data->ast_cmd->arg->data[i];
    struct string *obj = malloc(sizeof(struct string));
    obj->str = strdup(str);
    obj->index = 0;
    obj->len = strlen(str);
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

void my_strcat(char *dest, char *src)
{
    size_t i = 0;
    while (src[i])
    {
        dest[i] = src[i];
        i++;
    }
}

void destroy_string(struct string *str)
{
    free(str->str);
    free(str);
}
