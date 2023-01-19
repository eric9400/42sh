#ifndef EXECUTE_TOOLS_H
#define EXECUTE_TOOLS_H

#include <stddef.h>
#include "ast.h"

int is_char_variable(char c);
// void expandinho(char **str, int return_value, int *marker, size_t ind_marker);
int expandinho_phoenix(struct ast *ast);
char *expandinho_phoenix_junior(char *s);
char *is_special_var(char *str, int return_value);

#endif /* EXECUTE_TOOLS_H */
