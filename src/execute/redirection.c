#include <fnctl.h>
#include <stdio.h>

void restore_fd(struct stock_fd **list)
{
    struct stock_fd *tmp = NULL;

    while(*list)
    {
        tmp = (*list)->next;
        dup2((*list)->old_fd, (*list)->new_fd);
        free(*list);
        *list = tmp;
    }
    *list = NULL;
    return;
}

struct stock_fd *init_stock_fd(int old_fd, int new_fd, struct stock_fd *list)
{
    struct stock_fd *new = calloc(1, sizeof(struct stock_fd));
    new->old_fd = old_fd;
    new->new_fd = new_fd;
    new->next = list;
    return new;
}

int redir_s_right(struct ast *ast, struct stock_fd **list)
{
    int fd = open(ast->data->ast_redir->exit_file, O_WRONLY | O_TRUNC | O_CREAT); 
    if (fd == -1)
        return fd;
    int old_fd = dup(ast->io_number);

    dup2(fd, ast->io_number);

    *list = init_stock_fd(old_fd, ast->io_number);
    return 1;
}

int redir_s_left(struct ast *ast)
{

}
int redir_d_right(struct ast *ast)
{

}

int redir_right_and(struct ast *ast)
{

}

int redir_left_and(struct ast *ast)
{

}

int redir_right_pip(struct ast *ast)
{

}

int redir_left_right(struct ast *ast)
{

}
