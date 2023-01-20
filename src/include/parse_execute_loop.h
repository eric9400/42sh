#ifndef PARSE_EXECUTE_LOOP_H
#define PARSE_EXECUTE_LOOP_H

#include <stdio.h>

#include "utils.h"

int parse_execute_loop(FILE *f, struct flags *flags);
int freeAll(int error);

extern struct flags *global_flags;
extern struct lexer *lex;
extern struct ast *ast;
extern FILE *file;

#endif /* PARSE_EXECUTE_LOOP_H */
