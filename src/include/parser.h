#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>
#include "lexer.h"
#include "parser.h"
#include "token.h"
#include "ast.h"

struct ast *input(struct lexer *lex);
void pretty_print(struct ast *tree);

#endif /* PARSER_H */
