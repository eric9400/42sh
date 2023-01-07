#include "vector.h"

#include <stdio.h>
#include <stdlib.h>

struct vector *vector_init(size_t n)
{
    struct vector *init = malloc(sizeof(struct vector));

    if (!init)
    {
        return NULL;
    }

    init->data = malloc(n * sizeof(int));
    init->capacity = n;
    init->size = 0;

    return init;
}

void vector_destroy(struct vector *v)
{
    free(v->data);
    free(v);
}

struct vector *vector_resize(struct vector *v, size_t n)
{
    if (!v)
    {
        return NULL;
    }

    if (n < v->size)
    {
        v->size = n;
        v->data = realloc(v->data, n * sizeof(int));
        v->capacity = n;
        return v;
    }

    if (n == v->capacity)
    {
        return v;
    }

    v->data = realloc(v->data, n * sizeof(int));
    v->capacity = n;

    return v;
}

struct vector *vector_append(struct vector *v, int elt)
{
    if (!v)
    {
        return NULL;
    }

    if (v->size == v->capacity)
    {
        int n = v->capacity;
        v->data = realloc(v->data, n * 2 * sizeof(int));
        v->capacity *= 2;

        if (!v->data)
        {
            return NULL;
        }
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

    size_t i;
    for (i = 0; i < v->size - 1; i++)
    {
        printf("%d,", v->data[i]);
    }
    printf("%d\n", v->data[i]);
}

struct vector *vector_reset(struct vector *v, size_t n)
{
    free(v->data);
    v->data = malloc(sizeof(int) * n);

    if (!v->data)
    {
        return NULL;
    }

    v->size = 0;
    v->capacity = n;

    return v;
}

struct vector *vector_insert(struct vector *v, size_t i, int elt)
{
    if (!v)
    {
        return NULL;
    }

    if (i > v->size)
    {
        return NULL;
    }

    if (v->size == v->capacity)
    {
        int n = v->capacity;
        v->data = realloc(v->data, n * 2 * sizeof(int));
        if (!v->data)
        {
            return NULL;
        }
        v->capacity *= 2;
    }
    int shift = elt;
    int elm;
    for (size_t j = i; j < v->size + 1; j++)
    {
        elm = v->data[j];
        v->data[j] = shift;
        shift = elm;
    }
    v->size += 1;
    return v;
}

struct vector *vector_remove(struct vector *v, size_t i)
{
    if (!v)
    {
        return NULL;
    }

    if (i >= v->size)
    {
        return NULL;
    }

    if (i == v->size)
    {
        return v;
    }

    int pivot;
    for (size_t j = i; j < v->size - 1; j++)
    {
        pivot = v->data[j];
        v->data[j] = v->data[j + 1];
        v->data[j + 1] = pivot;
    }
    v->size -= 1;

    if (v->size >= 1 && v->size * 2 <= v->capacity)
    {
        int n = v->capacity / 2;
        v->data = realloc(v->data, n * sizeof(int));
        v->capacity /= 2;
    }

    return v;
}
