#ifndef UTILS_H
#define UTILS_H

#include "ast.h"

struct flags
{
    int c;
    int p;
    int u;
};

int BaBaJi(int argc, char **argv, char **filename, struct flags *flags);

#endif /* UTILS_H */
