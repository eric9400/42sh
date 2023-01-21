#ifndef PARSE_EXECUTE_LOOP_H
#define PARSE_EXECUTE_LOOP_H

#include <stdio.h>

#include "ast.h"
#include "lexer.h"
#include "utils.h"

struct toFree
{
    struct flags *global_flags;
    struct lexer *lex;
    struct ast *ast;
    FILE *file;
    char **env_variables;
};

extern struct toFree *tofree;

int freeAll(int error);
int parse_execute_loop(FILE *f, struct flags *flags);

#endif /* PARSE_EXECUTE_LOOP_H */
