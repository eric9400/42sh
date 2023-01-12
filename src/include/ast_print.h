#ifndef AST_PRINT_H
#define AST_PRINT_H

#include "ast.h"
#include "vector.h"

void pretty_print(struct ast *tree, int tab);
void ugly_print(struct ast *tree);

#endif /* AST_PRINT_H */