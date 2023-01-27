#ifndef HASH_MAP_GLOBAL_H
#define HASH_MAP_GLOBAL_H

#include "f_hash_map.h"
#include "hash_map.h"

struct hash_maps
{
    struct hash_map *hashmap;
    struct f_hash_map *fhashmap;
    struct hash_map *hashmap_alias;
};

extern struct hash_maps *hashM;

#endif /* HASH_MAP_GLOBAL_H */
