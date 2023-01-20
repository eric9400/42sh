#ifndef PARSER_CMD_SH_H
#define PARSER_CMD_SH_H

#include "parser.h"
#include "parser_cmd.h"

struct ast *shell_command(struct lexer *lex);

#endif /* PARSER_CMD_SH_H */