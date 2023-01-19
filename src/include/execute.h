#ifndef EXECUTE_H
#define EXECUTE_H

#include "ast.h"

int execute(struct ast *ast, int return_value);
int expandhino_phoenix(struct ast *ast);

#endif /* EXECUTE_H */
