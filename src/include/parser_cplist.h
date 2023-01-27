#ifndef PARSER_CPLIST_H
#define PARSER_CPLIST_H

#include "parser.h"
#include "parser_cmd.h"

struct ast *compound_list(struct lexer *lex);
struct ast *rule_case(struct lexer *lex);
void free_peek(struct lexer *lex);

#endif /* PARSER_CPLIST_H */