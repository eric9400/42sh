#include "hash_map_utils.h"

#include <stdio.h>

#include "hash_map.h"

void print_hash_map(void)
{
    int size = hashM->hashmap->size;
    for (int i = 0; i < size; i++)
    {
        printf("%d ->", i);
        if (hashM->hashmap->data != NULL)
        {
            struct pair_list *curr = hashM->hashmap->data[i];
            while (curr)
            {
                printf("( %s, %s) ", curr->key, curr->value);
                curr = curr->next;
            }
        }
        printf("\n");
    }
}
