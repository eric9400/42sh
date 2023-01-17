#ifndef REDIRECTION_H 
#define REDIRECTION_H

#include "ast.h"

struct stock_fd
{
    int old_fd;
    int new_fd;
    struct stock_fd *next;
};

int destroy_stock_fd(struct stock_fd *list);
int redir_s_right(struct ast *ast, struct stock_fd **list);
int redir_s_left(struct ast *ast);
int redir_d_right(struct ast *ast);
int redir_right_and(struct ast *ast);
int redir_left_and(struct ast *ast);
int redir_right_pip(struct ast *ast);
int redir_left_right(struct ast *ast);

#endif /* REDIRECTION_H */
