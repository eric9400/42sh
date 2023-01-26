#ifndef F_HASH_MAP_H
#define F_HASH_MAP_H

#include <stdbool.h>
#include <stddef.h>

#include "ast.h"

struct pair_ast {
  char *key;
  struct ast *value;
  struct pair_ast *next;
};

struct f_hash_map {
  struct pair_ast **data;
  size_t size;
};

struct f_hash_map *f_hash_map_init(size_t size);
bool f_hash_map_insert(struct f_hash_map *hash_map, const char *key,
                       struct ast *value);
void f_hash_map_free(struct f_hash_map *hash_map);
struct ast *f_hash_map_get(const struct f_hash_map *hash_map, const char *key);
bool f_hash_map_remove(struct f_hash_map *hash_map, const char *key);

#endif /* F_HASH_MAP_H */
