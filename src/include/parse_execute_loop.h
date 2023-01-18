#ifndef PARSE_EXECUTE_LOOP_H
#define PARSE_EXECUTE_LOOP_H

#include <stdio.h>

#include "utils.h"

int parse_execute_loop(FILE *file, struct flags *flags);
int freeAll(FILE *file, struct lexer *lex, struct ast *ast, int error);

extern struct flags *global_flags;
extern struct lexer *lex;
extern struct ast *ast;
extern FILE *file;

#endif /* PARSE_EXECUTE_LOOP_H */
