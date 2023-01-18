#ifndef PIPE_H
#define PIPE_H

#include "ast.h"

enum pipe { WRITE, READ };

int func_pipe(struct ast *ast, int return_value);

#endif /* PIPE_H */

