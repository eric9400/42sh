#include <stdlib.h>
#include <string.h>

#include "parser_cmd.h"

static void command2(struct ast_list *redir_list, struct lexer *lex);
static struct ast *funcdec(struct lexer *lex);

struct ast *command(struct lexer *lex)
{
    struct ast *cmd = simple_command(lex);

    if (cmd)
        return cmd;
    
    cmd = shell_command(lex);
    if (cmd)
    {
        if (cmd->type == AST_CMD)
            command2(cmd->data->ast_cmd->redir, lex);
        else if (cmd->type == AST_IF)
            command2(cmd->data->ast_if->redir, lex);
        else if (cmd->type == AST_WHILE)
            command2(cmd->data->ast_while->redir, lex);
        else if (cmd->type == AST_FOR)
            command2(cmd->data->ast_for->redir, lex);
        else if (cmd->type == AST_UNTIL)
            command2(cmd->data->ast_until->redir, lex);
        return cmd;
    }

    cmd = funcdec(lex);
    if (cmd)
    {
        command2(cmd->data->ast_func->redir, lex);
        return cmd;
    }

    return error_handler(lex, 0, "ERROR");
    //flag print is set to 0 to not print the error message
}

static void command2(struct ast_list *redir_list, struct lexer *lex)
{
    struct ast *redir = redirection(lex);
    if (!redir)
        return;
    
    add_to_list(redir_list, redir);
    command2(redir_list, lex);
}

static struct ast_redir *find_type_redir(struct lexer *lex, struct ast_redir *redir)
{
    if (!strcmp(">", lex->tok->data))
        redir->type = S_RIGHT;
    else if (!strcmp("<", lex->tok->data))
        redir->type = S_LEFT;
    else if (!strcmp(">>", lex->tok->data))
        redir->type = D_RIGHT;
    else if (!strcmp(">&", lex->tok->data))
        redir->type = RIGHT_AND;
    else if (!strcmp("<&", lex->tok->data))
        redir->type = LEFT_AND;
    else if (!strcmp(">|", lex->tok->data))
        redir->type = RIGHT_PIP; 
    else if (!strcmp("<>", lex->tok->data))
        redir->type = LEFT_RIGHT;
    else
    {
        if(redir->io_number != -1)
            error_handler(lex, 1, "ERROR REDIRECTION : FIND_TYPE_REDIR PROBLEM");
        free_node(convert_node_ast(AST_REDIR, redir));
        redir = NULL;
    }
    return redir;
}

static void default_ionb(struct lexer *lex, struct ast_redir *redir)
{
    if (redir->io_number != -1)
        return;
    if (lex->tok->data[0] == '>')
        redir->io_number = 1;
    else
        redir->io_number = 0;
}

struct ast *redirection(struct lexer *lex)
{
    peek_token(lex);

    struct ast_redir *redir = init_ast(AST_REDIR);

    if (lex->tok->type == IO_NUMBER)
    {
        redir->io_number = atoi(lex->tok->data);
        free_token(lex);
        peek_token(lex);
    }

    if (lex->tok->type != OPERATOR)
    {
        int io_number = redir->io_number;
        free_node(convert_node_ast(AST_REDIR, redir));
        if(io_number != -1)
            return error_handler(lex, 1, "ERROR REDIRECTION : LACK OF WORD");
        return NULL;
    }

    redir = find_type_redir(lex, redir);
    if (!redir)
        return NULL;
    
    default_ionb(lex, redir);
    free_token(lex);

    peek_token(lex);
    if (lex->tok->type != WORD && lex->tok->type != ASSIGNMENT_WORD)
    {
        free_node(convert_node_ast(AST_REDIR, redir));
        return error_handler(lex, 1, "ERROR REDIRECTION : LACK OF WORD");
    }

    redir->exit_file = strdup(lex->tok->data);
    free_token(lex);

    return convert_node_ast(AST_REDIR, redir);
}

static struct ast *funcdec(struct lexer *lex)
{
    peek_token(lex);

    if (lex->tok->type != WORD && lex->tok->type != ASSIGNMENT_WORD)
        return NULL;
    
    struct ast_func *ast_func = init_ast(AST_FUNC);
    ast_func->name = strdup(lex->tok->data);
    free_token(lex);

    peek_token(lex);
    if (lex->tok->type != OPERATOR || strcmp(lex->tok->data, "(") != 0)
    {
        free_node(convert_node_ast(AST_FUNC, ast_func));
        return error_handler(lex, 1, "ERROR FUNCDEX : LACK OF \"(\"");
    }
    free_token(lex);
    peek_token(lex);
    if (lex->tok->type != OPERATOR || strcmp(lex->tok->data, ")") != 0)
    {
        free_node(convert_node_ast(AST_FUNC, ast_func));
        return error_handler(lex, 1, "ERROR FUNCDEX : LACK OF \")\"");
    }

    new_lines(lex);

    ast_func->func = shell_command(lex);
    if (lex->error != 0 || !ast_func->func)
    {
        free_node(convert_node_ast(AST_FUNC, ast_func));
        return error_handler(lex, 1, "ERROR FUNCDEX : INVALID SHELL COMMAND");
    }

    return convert_node_ast(AST_FUNC, ast_func);
} 