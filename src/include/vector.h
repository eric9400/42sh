#ifndef VECTOR_H
#define VECTOR_H

#include <stddef.h>

struct vector *vector_init(size_t n);
void vector_destroy(struct vector *v);
struct vector *vector_remove(struct vector *v, size_t i);
struct vector *vector_resize(struct vector *v, size_t n);
struct vector *vector_append(struct vector *v, int elt);
void vector_print(const struct vector *v);
struct vector *vector_reset(struct vector *v, size_t n);

#endif /*VECTOR_H*/