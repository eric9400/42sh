#include "redirection.h"

#include <err.h>
#include <fnctl.h>
#include <stdio.h>

void destroy_stock_fd(struct stock_fd *list)
{
    struct stock_fd *tmp = NULL;
    while(list)
    {
        tmp = list->next;
        dup2(list->old_fd, list->new_fd);
        close(list->old_fd);
        free(list);
        list = tmp;
    }
}

static struct stock_fd *prepend_stock_fd(int old_fd, int new_fd, struct stock_fd **list)
{
    struct stock_fd *new = calloc(1, sizeof(struct stock_fd));
    new->old_fd = old_fd;
    new->new_fd = new_fd;
    new->next = *list;
    return new;
}

// 1>toto.txt
int redir_s_right(struct ast *ast, struct stock_fd **list, int GDDN)
{
    int fd = GDDN;
    if (GDDN == -1)
        fd = open(ast->data->ast_redir->exit_file, O_WRONLY | O_TRUNC | O_CREAT); 
    if (fd == -1)
        return 1;
    int fd_dup = dup(ast->io_number);
    int new_io = dup2(fd, ast->io_number);
    close(fd);
    *list = prepend_stock_fd(fd_dup, ast->io_number, list);
    return 0;
}

// 1<toto.txt
int redir_s_left(struct ast *ast, struct stock_fd **list)
{
    int fd = open(ast->data->ast_redir->exit_file, O_RDONLY | O_APPEND); 
    if (fd == -1)
        return 1;
    int fd_dup = dup(fd);
    dup2(ast->io_number, fd);
    close(fd);
    *list = prepend_stock_fd(fd_dup, fd, list);
    return 0;
}

// 1>>toto.txt
int redir_d_right(struct ast *ast, struct stock_fd **list)
{
    int fd = open(ast->data->ast_redir->exit_file, O_WRONLY | O_CREAT | O_APPEND);
    if (fd == -1)
        return 1;
    int fd_dup = dup(ast->io_number);
    dup2(fd, ast->io_number);
    close(fd);
    *list = prepend_stock_fd(fd_dup, ast->io_number, list);
    return 0;
}

// 1>&toto.txt
int redir_right_and(struct ast *ast, struct stock_fd **list)
{
    errno = 0;
    int temp = atoi(ast->data->ast_redir->exit_file);
    // file
    if (errno != 0)
    {
        if (strcmp(ast->data->ast_redir->exit_file, "-"))
            return redir_right(ast, list);
        close(ast->data->ast_redir->io_number);
        return 0;

    }
    // filedescriptor 1>&2 for example
    else
    {
        
    }
}

// 1<&2
int redir_left_and(struct ast *ast, struct stock_fd **list)
{
    if (strcmp(ast->data->ast_redir->exit_file, "-"))
        return redir_right(ast, list);
    close(ast->data->ast_redir->io_number);
    return 0;
}

// 1>|toto.txt
int redir_right_pip(struct ast *ast, struct stock_fd **list)
{
    int fd = open(ast->data->ast_redir->exit_file, O_WRONLY | O_TRUNC | O_CREAT); 
    if (fd == -1)
        return;
    int fd_dup = dup(ast->io_number);
    dup2(fd, ast->io_number);
    *list = prepend_stock_fd(fd_dup, ast->io_number, list);
}

// 1<>toto.txt
int redir_left_right(struct ast *ast, struct stock_fd **list)
{
    int fd = open(ast->data->ast_redir->exit_file, O_WRONLY | O_TRUNC | O_CREAT); 
    if (fd == -1)
        return;
    int fd_dup = dup(ast->io_number);
    dup2(fd, ast->io_number);
    *list = prepend_stock_fd(fd_dup, ast->io_number, list);
}
