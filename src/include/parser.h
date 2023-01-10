#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include "token.h"
#include "ast.h"

struct ast *input(struct lexer *lex);

#endif /* PARSER_H */
