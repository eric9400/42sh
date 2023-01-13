#ifndef AST_INIT_H
#define AST_INIT_H

#include "ast.h"
#include "vector.h"

struct ast_cmd *init_cmd(void);
struct ast_list *init_list(size_t capacity);
struct ast_if *init_if(void);

#endif /* AST_INIT_H */