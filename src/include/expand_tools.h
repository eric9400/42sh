#ifndef EXPAND_TOOLS_H
#define EXPAND_TOOLS_H

#include "my_string.h"

void string_append(struct string *new_str, char *buf);
int dollar_expansion(struct string *str, struct string *new_str);
void slash_expansion_in_d_quotes(struct string *str, struct string *new_str);

#endif /* EXPAND_TOOLS_H */
