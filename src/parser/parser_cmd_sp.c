#include "parser.h"

#include <stdlib.h>
#include <string.h>

struct ast *simple_command(struct lexer *lex);
static char *element(struct lexer *lex);
static void simple_command2(struct lexer *lex, struct ast_cmd *cmd);

struct ast *simple_command(struct lexer *lex)
{
    struct ast_cmd *cmd = init_cmd();

    peek_token(lex);
    if (lex->tok->type != WORD || is_shell_command(lex))
    {
        free_node(convert_node_ast(AST_CMD, cmd));
        return NULL;
    }

    char *word = strdup(lex->tok->data);
    vector_append(cmd->arg, word);
    free_token(lex);

    simple_command2(lex, cmd);

    return convert_node_ast(AST_CMD, cmd);
}

static void simple_command2(struct lexer *lex, struct ast_cmd *cmd)
{
    char *word = element(lex);
    vector_append(cmd->arg, word);

    if (word == NULL)
        return;
    simple_command2(lex, cmd);
}

static char *element(struct lexer *lex)
{
    peek_token(lex);
    if (lex->tok && lex->tok->type == WORD)
    {
        char *word = strdup(lex->tok->data);
        free_token(lex);
        return word;
    }
    return NULL;
}