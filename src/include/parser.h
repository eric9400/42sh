#ifndef PARSER_H
#define PARSER_H

#include "ast.h"
#include "ast_free.h"
#include "ast_init.h"
#include "ast_print.h"
#include "lexer.h"
#include "parser_cmd_sh.h"
#include "parser_cmd_sp.h"
#include "token.h"
#include "utils.h"

struct ast *input(struct lexer *lex);
struct ast *and_or(struct lexer *lex);

// UTILS
void new_lines(struct lexer *lex);
struct ast *convert_node_ast(enum ast_type type, void *node);
struct ast *error_handler(struct lexer *lex, int print, char *error_message);

#endif /* PARSER_H */