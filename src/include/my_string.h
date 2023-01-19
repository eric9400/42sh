#ifndef MY_STRING_H
#define MY_STRING_H

#include <stddef.h>

struct string 
{
    char *str;
    size_t index;
    size_t len;
};

#endif /* MY_STRING_H */
