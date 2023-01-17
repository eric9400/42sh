#include "redirection.h"

#include <err.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

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
int redir_s_right(struct ast *ast, struct stock_fd **list)
{
    int fd = atoi(ast->data->ast_redir->exit_file); //MIGHT CHANGE HERE IF exit_file == "0"
    if (fd == 0) //FILE
        fd = open(ast->data->ast_redir->exit_file, O_TRUNC | O_CREAT | O_WRONLY, 00666); 
    if (fd == -1)
        return 1;
    int fd_dup = dup(ast->data->ast_redir->io_number);
    dup2(fd, ast->data->ast_redir->io_number);
    close(fd);
    *list = prepend_stock_fd(fd_dup, ast->data->ast_redir->io_number, list);
    return 0;
}

// 1<toto.txt
int redir_s_left(struct ast *ast, struct stock_fd **list, int GDDN)
{
    int fd = atoi(ast->data->ast_redir->exit_file);
    if (fd == 0) //IF NOT A NUMBER
    {
        if (GDDN) //IF EXPECT A NUMBER ON RIGHT SIDE (WHEN 1<&2)
            return 1;
        else
            fd = open(ast->data->ast_redir->exit_file, O_APPEND | O_RDONLY);
    }
    if (fd == -1)
        return 1;
    int fd_dup = dup(ast->data->ast_redir->io_number);
    dup2(fd, ast->data->ast_redir->io_number);
    close(fd);
    *list = prepend_stock_fd(fd_dup, ast->data->ast_redir->io_number, list);
    return 0;
}

// 1>>toto.txt
int redir_d_right(struct ast *ast, struct stock_fd **list)
{
    int fd = atoi(ast->data->ast_redir->exit_file);
    if (fd == 0) 
        fd = open(ast->data->ast_redir->exit_file, O_CREAT | O_APPEND | O_WRONLY, 00666);
    if (fd == -1)
        return 1;
    int fd_dup = dup(ast->data->ast_redir->io_number);
    dup2(fd, ast->data->ast_redir->io_number);
    close(fd);
    *list = prepend_stock_fd(fd_dup, ast->data->ast_redir->io_number, list);
    return 0;
}

// 1>&toto.txt 1>&- 1>&2
int redir_right_and(struct ast *ast, struct stock_fd **list)
{
    if (strcmp(ast->data->ast_redir->exit_file, "-"))
        return redir_s_right(ast, list);
    close(ast->data->ast_redir->io_number);
    return 0;
}

// 1<&toto.txt 1<&-
int redir_left_and(struct ast *ast, struct stock_fd **list)
{
    if (strcmp(ast->data->ast_redir->exit_file, "-"))
        return redir_s_left(ast, list, 1);
    close(ast->data->ast_redir->io_number);
    return 0;
}

// 1>|toto.txt
int redir_right_pip(struct ast *ast, struct stock_fd **list)
{
    return redir_s_right(ast, list);   
}

// echo toto <> test
// 1<>toto.txt
int redir_left_right(struct ast *ast, struct stock_fd **list)
{ 
    //MIGHT CHANGE HERE IF exit_file == "0"
    int fd = atoi(ast->data->ast_redir->exit_file);
    if (fd == 0) //FILE
        fd = open(ast->data->ast_redir->exit_file, O_CREAT | O_RDWR, 00666); 
    if (fd == -1)
        return 1;
    int fd_dup = dup(ast->data->ast_redir->io_number);
    dup2(fd, ast->data->ast_redir->io_number);
    close(fd);
    *list = prepend_stock_fd(fd_dup, ast->data->ast_redir->io_number, list);
    return 0;
}
