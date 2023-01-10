#ifndef UTILS_H
#define UTILS_H

#include "ast.h"

struct flags
{
    int c;
    int p;
};

int BaBaJi(int argc, char *argv[], char **filename, struct flags *flags);
void pretty_print(struct ast *tree, int tab);

#endif /* UTILS_H */

