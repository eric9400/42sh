#ifndef MY_STRING_H
#define MY_STRING_H

#include <stddef.h>

struct string 
{
    char *str;
    size_t index;
    size_t len;
};

struct string *init_string(char *str, size_t index, size_t len);
void destroy_string(struct string *str);
void my_strcat(char *dest, char *src);

#endif /* MY_STRING_H */
