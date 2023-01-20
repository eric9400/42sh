#ifndef VECTOR_H
#define VECTOR_H

#include <stddef.h>

struct vector
{
    size_t size;
    size_t capacity;
    char **data;
};

struct vector *vector_init(size_t n);

void vector_destroy(struct vector *v);
struct vector *vector_remove(struct vector *v, size_t i);
struct vector *vector_resize(struct vector *v, size_t n);
struct vector *vector_append(struct vector *v, char *s);
void vector_print(const struct vector *v);
struct vector *vector_reset(struct vector *v, size_t n);
struct vector *vector_copy(struct vector *v);

#endif /* VECTOR_H */
