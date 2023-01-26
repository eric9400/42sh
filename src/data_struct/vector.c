#include "vector.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct vector *vector_init(size_t n)
{
    struct vector *init = malloc(sizeof(struct vector));

    if (!init)
        return NULL;

    init->data = malloc(n * sizeof(char *));
    init->capacity = n;
    init->size = 0;

    return init;
}

void vector_destroy(struct vector *v)
{
    for (size_t i = 0; i < v->size; i++)
        free(v->data[i]);
    free(v->data);
    free(v);
}

struct vector *vector_resize(struct vector *v, size_t n)
{
    if (!v)
        return NULL;

    if (n < v->size)
    {
        v->size = n;
        v->data = realloc(v->data, n * sizeof(char *));
        v->capacity = n;
        return v;
    }

    if (n == v->capacity)
        return v;

    v->data = realloc(v->data, n * sizeof(char *));
    v->capacity = n;

    return v;
}

// s already allocated
struct vector *vector_append(struct vector *v, char *elt)
{
    if (!v)
        return NULL;

    if (v->size == v->capacity)
    {
        v->capacity *= 2;
        v->data = realloc(v->data, v->capacity * sizeof(char *));
        if (!v->data)
            return NULL;
    }

    v->data[v->size] = elt;
    v->size += 1;

    return v;
}

void vector_print(const struct vector *v)
{
    if (v->size == 0)
    {
        printf("\n");
        return;
    }

    for (size_t i = 0; i < v->size; i++)
        printf("%ld. %s ", i + 1, v->data[i]);
}

struct vector *vector_reset(struct vector *v, size_t n)
{
    for (size_t i = 0; i < v->size; i++)
        free(v->data[i]);
    free(v->data);

    v->data = malloc(sizeof(char *) * n);

    if (!v->data)
        return NULL;

    v->size = 0;
    v->capacity = n;

    return v;
}

// 14 lines
struct vector *vector_copy(struct vector *v, int is_for)
{
    struct vector *res = malloc(sizeof(struct vector));
    res->size = v->size;
    res->capacity = v->capacity;
    res->data = malloc(sizeof(char *) * v->capacity);
    int bound = v->size;
    if (!is_for)
        bound = v->size - 1;
    for (int i = 0; i < bound; i++)
        res->data[i] = strdup(v->data[i]);
    if (!is_for)
        res->data[v->size - 1] = NULL;
    else
        res->data[v->size] = NULL;
    return res;
}
