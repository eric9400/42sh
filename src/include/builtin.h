#ifndef BUILTIN_H
#define BUILTIN_H

#include "execute.h"

int echo(char **s);
int check_builtin(char **str, struct c_or_b *no_to_racismo, int return_value);

extern int is_in_dot;

#endif /* BUILTIN_H */
