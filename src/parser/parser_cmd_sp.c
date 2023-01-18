#include <stdlib.h>
#include <string.h>

#include "parser.h"

struct ast *simple_command(struct lexer *lex);
static int element(struct lexer *lex, struct ast_cmd *cmd);
static int prefix(struct lexer *lex, struct ast_cmd *cmd);
static void simple_command2(struct lexer *lex, struct ast_cmd *cmd);
static void simple_command3(struct lexer *lex, struct ast_cmd *cmd);

int is_shell_command(struct lexer *lex)
{
    char *data = lex->tok->data;
    return strcmp("if", data) == 0 || strcmp("else", data) == 0
        || strcmp("elif", data) == 0 || strcmp("then", data) == 0
        || strcmp("fi", data) == 0 || strcmp("while", data) == 0 
        || strcmp("until", data) == 0 || strcmp("for", data) == 0 
        || strcmp("do", data) == 0 || strcmp("done", data) == 0;
}

struct ast *simple_command(struct lexer *lex)
{
    struct ast_cmd *cmd = init_ast(AST_CMD);
    simple_command2(lex, cmd);

    peek_token(lex);
    if (lex->tok->type == END_OF_FILE || lex->tok->type == NEWLINE
        || lex->tok->type == SEMICOLON)
    {
        if (lex->tok->type == END_OF_FILE || lex->tok->type == NEWLINE)
            vector_append(cmd->arg, strdup(""));
        vector_append(cmd->arg, NULL);
        return convert_node_ast(AST_CMD, cmd);
    }

    if ((lex->tok->type != WORD || is_shell_command(lex))
         && lex->tok->type != END_OF_FILE && lex->tok->type != NEWLINE)
    {
        free_node(convert_node_ast(AST_CMD, cmd));
        return NULL;
    }
    
    char *word = strdup(lex->tok->data);
    vector_append(cmd->arg, word);
    free_token(lex);

    simple_command3(lex, cmd);

    return convert_node_ast(AST_CMD, cmd);
}

static void simple_command2(struct lexer *lex, struct ast_cmd *cmd)
{
    int stop = prefix(lex, cmd);
    if (stop)
        return;
    simple_command2(lex, cmd);
}

static void simple_command3(struct lexer *lex, struct ast_cmd *cmd)
{
    int stop = element(lex, cmd);
    if (stop)
        return;
    simple_command3(lex, cmd);
}

static int prefix(struct lexer *lex, struct ast_cmd *cmd)
{
    peek_token(lex);
    if (lex->tok->type == ASSIGNMENT_WORD)
    {
        vector_append(cmd->arg, strdup(lex->tok->data));
        free_token(lex);
        return 0;
    }

    struct ast *redir = redirection(lex);
    if (!redir)
        return 1;

    add_to_list(cmd->redir, redir);
    return 0;
}

static int element(struct lexer *lex, struct ast_cmd *cmd)
{
    peek_token(lex);
    if (lex->tok->type == WORD)
    {
        vector_append(cmd->arg, strdup(lex->tok->data));
        free_token(lex);
        return 0;
    }

    struct ast *redir = redirection(lex);
    if (!redir)
    {
        vector_append(cmd->arg, NULL);
        return 1;
    }

    add_to_list(cmd->redir, redir);
    return 0;
}
