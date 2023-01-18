#ifndef EXECUTE_H
#define EXECUTE_H

#include "ast.h"

struct c_or_b 
{
    int is_in_loop = 0;
    int loop_deep = 0;
    int cbdeep = 0;
    int is_break = -1;
};

int execute(struct ast *ast, int return_value);

#endif /* EXECUTE_H */
