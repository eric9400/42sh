#ifndef EXECUTE_TOOLS_H
#define EXECUTE_TOOLS_H

#include "ast.h"
#include "vector.h"

struct string 
{
    char *str;
    size_t index;
    size_t len;
};

void split_and_append(struct vector **v, char *s);
void split_vector(int *marker, struct ast *ast);

void split_quote_at(struct vector **v);
void split_quote_star(struct vector **v);
void split_no_quote(struct vector **v);

int is_char_variable(char c);
void expandinho(char **str, int return_value, int *marker, size_t ind_marker);

#endif /* EXECUTE_TOOLS_H */
