#include "pipe.h"

#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "execute.h"
#include "lexer.h"

// 18 lines
static pid_t exec_pipe(struct ast *ast, int fds[2], enum pipe side,
                       int return_value)
{
    pid_t pid = fork();
    if (pid != 0)
        return pid;

    // fds[0] = READ & fds[1] = WRITE;
    close(fds[side]);

    int old_dup;
    if (side == WRITE)
    {
        old_dup = dup(STDOUT_FILENO);
        dup2(fds[1 - side], STDOUT_FILENO);
    }
    else
    {
        old_dup = dup(STDIN_FILENO);
        dup2(fds[1 - side], STDIN_FILENO);
    }
    close(fds[1 - side]);

    int res = execute(ast, return_value);

    // restore
    if (side == WRITE)
        dup2(old_dup, STDOUT_FILENO);
    else
        dup2(old_dup, STDIN_FILENO);

    exit(res);
}

// 16 lines
int func_pipe(struct ast *ast, int return_value)
{
    int pipe_fds[2]; // file descriptor used to redirect stdin & stdout

    if (pipe(pipe_fds) < 0)
        return 1;

    pid_t left_pid = -1;
    pid_t right_pid = -1;

    int status1;
    int status2;
    left_pid =
        exec_pipe(ast->data->ast_pipe->left, pipe_fds, WRITE, return_value);
    right_pid =
        exec_pipe(ast->data->ast_pipe->right, pipe_fds, READ, return_value);
    close(pipe_fds[0]);
    close(pipe_fds[1]);
    waitpid(right_pid, &status1, 0);
    waitpid(left_pid, &status2, 0);

    return WEXITSTATUS(status1);
}
