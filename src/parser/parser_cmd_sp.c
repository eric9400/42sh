#include "parser.h"

#include <stdlib.h>
#include <string.h>

struct ast *simple_command(struct lexer *lex);
static struct ast *element(struct lexer *lex);
//static void simple_command2(struct lexer *lex, struct ast_cmd *cmd);

struct ast *simple_command(struct lexer *lex)
{
    struct ast_sp_cmd *cmd = init_sp_cmd();

    struct ast *pref = prefix(lex);

    while(pref)
    {
        cmd->cmd_prefix[cmd->size_prefix] = pref;
        cmd->size_prefix++;
        pref = prefix(lex);
    }

    peek_token(lex);

    if (lex->tok->type != WORD)
        return convert_node_ast(AST_SP_CMD, cmd);

    cmd->word = strdup(lex->tok->data);

    free_token(lex);

    struct ast *elmt = element(lex);

    while (elmt)
    {
        cmd->cmd_element[cmd->size_element] = elmt;
        cmd->size_element++;
        elmt = element(lex);
    }

    return convert_node_ast(AST_SP_CMD, cmd);
}

/*struct ast *simple_command(struct lexer *lex)
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
*/

static struct ast *element(struct lexer *lex)
{
    peek_token(lex);

    struct ast_element *elmt = init_element();

    if (lex->tok->type == WORD)
    {
        elmt->word = strdup(lex->tok->data);
        free_token(lex);
        return convert_node_ast(AST_ELEMENT,elmt);
    }

    elmt->redir = redirection(lex);

    if (elmt->redir)
        return convert_node_ast(AST_ELEMENT,elmt);
    
    free_node(convert_node_ast(AST_ELEMENT,elmt));
    return NULL;
}