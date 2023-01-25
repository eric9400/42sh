#ifndef EXECUTE_TOOLS_H
#define EXECUTE_TOOLS_H

#include <stddef.h>

#include "ast.h"

int expandinho_phoenix(struct ast *ast, int return_value);
char *expandinho_phoenix_junior(char *s, int return_value);
int check_function(char **str, int return_value);

#endif /* EXECUTE_TOOLS_H */
