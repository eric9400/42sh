#ifndef PARSE_EXECUTE_LOOP_H
#define PARSE_EXECUTE_LOOP_H

#include <stdio.h>

#include "utils.h"

extern struct flags *global_flags;
extern struct lexer *lex;
extern struct ast *ast;
extern FILE *file;

int freeAll(int error);
int parse_execute_loop(FILE *f, struct flags *flags);

#endif /* PARSE_EXECUTE_LOOP_H */
