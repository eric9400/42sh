#ifndef EXPAND_TOOLS_H
#define EXPAND_TOOLS_H

#include "my_string.h"

void string_append(struct string *new_str, char *buf);
int dollar_expansion(struct string *str, struct string *new_str,
                     int return_value, int in_d_quotes);
void slash_expansion_in_d_quotes(struct string *str, struct string *new_str,
                                 int in_d_quotes);
int command_substitution(struct string *str, struct string *new_str, char delim);

#endif /* EXPAND_TOOLS_H */
