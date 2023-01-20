#ifndef PARSER_CMD_SP_H
#define PARSER_CMD_SP_H

#include "parser.h"
#include "parser_cmd.h"

int is_shell_command(struct lexer *lex);
struct ast *simple_command(struct lexer *lex);

#endif /* PARSER_CMD_SP_H */