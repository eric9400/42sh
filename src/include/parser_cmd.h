#ifndef PARSER_CMD_H
#define PARSER_CMD_H

#include "parser.h"
#include "parser_cmd_sh.h"
#include "parser_cmd_sp.h"

struct ast *command(struct lexer *lex);
struct ast *redirection(struct lexer *lex);

#endif /* PARSER_CMD_H */
