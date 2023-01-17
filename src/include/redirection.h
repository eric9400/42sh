#ifndef REDIRECTION_H 
#define REDIRECTION_H

struct stock_fd
{
    int old_fd;
    int new_fd;
    struct stock_fd *next;
};

#endif /* REDIRECTION_H */

