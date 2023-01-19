#include "my_string.h"

#include <stdlib.h>

struct string *init_string(char *str, size_t index, size_t len)
{
    struct string *obj = malloc(sizeof(struct string));
    obj->str = strdup(str);
    obj->index = index;
    obj->len = len;
    return obj;
}

void destroy_string(struct string *str)
{
    free(str->str);
    free(str);
}
