#ifndef AST_FREE_H
#define AST_FREE_H

#include "ast.h"
#include "vector.h"

void free_node(struct ast *ast);
void free_func(struct ast *ast);

#endif /* AST_FREE_H */
