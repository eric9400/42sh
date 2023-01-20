#include "parser.h"

#include <stdlib.h>
#include <string.h>

struct ast *input(struct lexer *lex);
static struct ast *list(struct lexer *lex);
static void list2(struct lexer *lex, struct ast_list *exec_tree);
struct ast *and_or(struct lexer *lex);
static struct ast *pipeline(struct lexer *lex);

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
    peek_token(lex);
    if (lex->tok->type == NEWLINE || lex->tok->type == END_OF_FILE)
        return;

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

    if (lex->tok->type != OPERATOR || (strcmp(lex->tok->data, "||") != 0
    && strcmp(lex->tok->data, "&&") != 0))
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