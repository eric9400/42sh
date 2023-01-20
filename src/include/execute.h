#ifndef EXECUTE_H
#define EXECUTE_H

#include "ast.h"

struct c_or_b 
{
    int is_in_loop;
    int loop_deep;
    int cbdeep;
    int is_break;
};

int execute(struct ast *ast, int return_value);
int expandhino_phoenix(struct ast *ast);

#endif /* EXECUTE_H */
