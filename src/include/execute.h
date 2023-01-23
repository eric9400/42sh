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
struct stock_fd *func_redir(struct ast_list *redir, int return_value, int *error);

#endif /* EXECUTE_H */
