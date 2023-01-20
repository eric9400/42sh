#ifndef MY_STRING_H
#define MY_STRING_H

#include <stddef.h>

#include "ast.h"
#include "vector.h"

struct string 
{
    char *str;
    size_t index;
    size_t len;
    struct vector *v;
};

struct string *init_string(struct ast *ast, int i, struct vector *v);
struct string *init_string2(char *str, size_t index, size_t len);
struct string *init_string3(size_t len);
void destroy_string(struct string *str);
void my_strcat(char *dest, char *src);

#endif /* MY_STRING_H */
