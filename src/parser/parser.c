#include "parser.h"

#include <stdlib.h>
#include <string.h>

struct ast *input(struct lexer *lex);
static struct ast *list(struct lexer *lex);
static void list2(struct lexer *lex, struct ast_list *exec_tree);
struct ast *and_or(struct lexer *lex);
static struct ast *pipeline(struct lexer *lex);
static struct ast *command(struct lexer *lex);
static void command2(struct ast_list *redir_list, struct lexer *lex);

/*
 * check if the current token is a shell command
 */
void new_lines(struct lexer *lex)
{
    peek_token(lex);
    while (lex->tok->type == NEWLINE)
    {
        free_token(lex);
        peek_token(lex);
    }
}

/*
 *Error handler puts an error in the "lex struct" and print the "error_message"
 *only if the flag "print" is 1
 */
struct ast *error_handler(struct lexer *lex, int print, char *error_message)
{
    lex->error = 2;
    char var = error_message[0];
    var++;
    if (print)
        fprintf(stderr, "%s\n", error_message);
    return NULL;
}

struct ast *input(struct lexer *lex)
{
    peek_token(lex);
    if (lex->tok->type == END_OF_FILE) // EOF
    {
        free_token(lex);
        return NULL; // ast vide
    }
    if (lex->tok->type == NEWLINE) // Si on a une ligne vide
    {
        free_token(lex);
        struct ast_cmd *cmd = init_ast(AST_CMD);
        vector_append(cmd->arg, strdup("")); // faire la modif
        return convert_node_ast(AST_CMD, cmd);
    }

    struct ast *exec_tree = list(lex);
    if (lex->error == 2)
    {
        error_handler(lex, 1, "ERROR INPUT: NO MATCHING PATERN");
        return exec_tree;
    }

    peek_token(lex);
    if (lex->tok->type == END_OF_FILE || lex->tok->type == NEWLINE)
    {
        free_token(lex);
        return exec_tree;
    }
    lex->error = 2;
    return exec_tree;
}

static struct ast *list(struct lexer *lex)
{
    struct ast_list *exec_tree = init_ast(AST_LIST);

    struct ast *head_cmd = and_or(lex);
    if (head_cmd != NULL)
        add_to_list(exec_tree, head_cmd);
    if (lex->error == 2)
    {
        error_handler(lex, 1, "ERROR LIST: NO MATCHING PATERN");
        return convert_node_ast(AST_LIST, exec_tree);
    }

    list2(lex, exec_tree);

    // plusieurs commandes donc une list
    if (exec_tree->size > 1)
        return convert_node_ast(AST_LIST, exec_tree);

    // une seule commande
    free(exec_tree->cmd_if);
    free(exec_tree);
    return head_cmd;
}

/*
 *If there is more than one ast than we add it in the list until it doesn't fit
 */
static void list2(struct lexer *lex, struct ast_list *exec_tree)
{
    peek_token(lex);
    if (lex->tok->type != SEMICOLON)
        return;

    free_token(lex);
    struct ast *cmd = and_or(lex);
    if (!cmd) // If we are at the end of the list
    {
        lex->error = 0;
        return;
    }

    add_to_list(exec_tree, cmd);
    list2(lex, exec_tree);
}

struct ast *and_or(struct lexer *lex)
{
    struct ast *pipe = pipeline(lex);
    
    if (!pipe)
        return NULL;

    peek_token(lex);

    if (lex->tok->type != OPERATOR)
        return pipe;
    
    struct ast *child = NULL;

    while (1)
    {
        if (!strcmp(lex->tok->data, "&&"))
        {
            free_token(lex);
            struct ast_and *ast_op = init_ast(AST_AND);
            new_lines(lex);

            struct ast *pipe2 = pipeline(lex);
            if (!pipe2)
            {
                free_node(convert_node_ast(AST_AND, ast_op));
                return error_handler(lex, 1, "Error and_or: inside && : NO MATCHING PATERN\n");
            }
            ast_op->right = pipe2;

            if (pipe)
            {
                ast_op->left = pipe;
                pipe = NULL;
            }
            else
                ast_op->left = child;
            child = convert_node_ast(AST_AND, ast_op);
        }

        else if (!strcmp(lex->tok->data, "||"))
        {
            free_token(lex);
            struct ast_or *ast_op = init_ast(AST_OR);
            new_lines(lex);

            struct ast *pipe2 = pipeline(lex);
            if (!pipe2)
            {
                free_node(convert_node_ast(AST_OR, ast_op));
                return error_handler(lex, 1, "Error and_or: inside || : NO MATCHING PATERN\n");
            }
            ast_op->right = pipe2;

            if (pipe)
            {
                ast_op->left = pipe;
                pipe = NULL;
            }
            else
                ast_op->left = child;
            child = convert_node_ast(AST_OR, ast_op);
        }

        else
            break;
    }
    return child;
}

static struct ast *pipeline(struct lexer *lex)
{
    peek_token(lex);

    struct ast_not *not = NULL;

    if (lex->tok->type == OPERATOR && !strcmp(lex->tok->data, "!"))
    {    
        free_token(lex);
        not = init_ast(AST_NOT);
    }

    struct ast *cmd = command(lex);
    if (!cmd)
    {
        if (not)
        {
            free_node(convert_node_ast(AST_NOT, not));
            return error_handler(lex, 1, "ERROR PIPELINE : CMD 1 NO MATCHING PATTERN");
        }
        return NULL;
    }

    peek_token(lex);

    struct ast *parent = NULL;

    parent = cmd;

    while(lex->tok->type == OPERATOR && !strcmp(lex->tok->data, "|"))
    {
        free_token(lex);

        new_lines(lex);

        struct ast *cmd2 = command(lex);

        if (!cmd2)
        {
            free_node(parent); //l'implémenter !!!!
            return error_handler(lex, 1, "ERROR PIPELINE : CMD 2 NO MATCHING PATTERN");
        }

        struct ast_pipe *pipe = init_ast(AST_PIPE);

        pipe->left = parent;
        pipe->right = cmd2;
        parent = convert_node_ast(AST_PIPE, pipe);
    }

    if (not)
    {
        not->node = parent;
        return convert_node_ast(AST_NOT, not);
    }
    return parent;
}

static struct ast *command(struct lexer *lex)
{
    struct ast *cmd = simple_command(lex);

    if (!cmd)
    {
        cmd = shell_command(lex);

        if (!cmd)
            return error_handler(lex, 0, "ERROR");
            //flag print is set to 0 to not print the error message
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
    }
    return cmd;
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
    if (lex->tok->type != WORD)
    {
        free_node(convert_node_ast(AST_REDIR, redir));
        return error_handler(lex, 1, "ERROR REDIRECTION : LACK OF WORD");
    }

    redir->exit_file = strdup(lex->tok->data);
    free_token(lex);

    return convert_node_ast(AST_REDIR, redir);
}
