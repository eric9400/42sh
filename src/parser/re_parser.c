#include "parser.h"

#include <stdlib.h>
#include <string.h>

#define SIZE 100

// static struct lexer *lex = NULL;
// static int error;

static struct ast *list(struct lexer *lex);
struct ast *input(struct lexer *lex);
static struct ast *list(struct lexer *lex);
static struct ast *and_or(struct lexer *lex);
static struct ast *pipeline(struct lexer *lex);
static struct ast *command(struct lexer *lex);
static struct ast *simple_command(struct lexer *lex);
static char *element(struct lexer *lex);
static struct ast *rule_if(struct lexer *lex, int opt);
static struct ast *shell_command(struct lexer *lex);
static struct ast *else_clause(struct lexer *lex);
static struct ast *compound_list(struct lexer *lex);

/*
 * check if the current token is a simple command
 */

int is_shell_command(struct lexer *lex)
{
    char *data = lex->tok->data;
    return strcmp("if", data) == 0 || strcmp("else", data) == 0
        || strcmp("elif", data) == 0 || strcmp("then", data) == 0
        || strcmp("fi", data) == 0;
}

/*
 *convert any type of ast into a general ast
 */
static struct ast *convert_node_ast(enum ast_type type, void *node)
{
    struct ast *ast_node = calloc(1, sizeof(struct ast));
    ast_node->data = calloc(1, sizeof(union ast_union));
    ast_node->type = type;

    if (type == AST_LIST)
        ast_node->data->ast_list = (struct ast_list *)node;
    if (type == AST_CMD)
        ast_node->data->ast_cmd = (struct ast_cmd *)node;
    if (type == AST_IF)
        ast_node->data->ast_if = (struct ast_if *)node;
    // ADD NEW AST CONVERT HERE

    return ast_node;
}

static void peek_token(struct lexer *lex)
{
    if (!lex->tok)
        next_token(lex);
}

static struct ast *error_handler(struct lexer *lex, char *error_message)
{
    lex->error = 2;
    char var = error_message[0];
    var++;
    // printf("%s\n", error_message);
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
        struct ast_cmd *cmd = init_cmd();
        vector_append(cmd->arg, strdup(""));
        return convert_node_ast(AST_CMD, cmd);
    }

    struct ast *exec_tree = list(lex);
    if (lex->error == 2)
    {
        // printf("Error input: NO MATCHING PATERN\n");
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
    int cpt_cmd = 0;
    struct ast_list *exec_tree = init_list(SIZE);

    struct ast *head_cmd = and_or(lex);
    if (head_cmd != NULL)
    {
        exec_tree->cmd_if[exec_tree->size] = head_cmd;
        exec_tree->size++;
    }
    if (lex->error == 2)
        return convert_node_ast(AST_LIST, exec_tree);
        // printf("Error list: NO MATCHING PATERN\n");

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

    exec_tree->cmd_if[exec_tree->size] = cmd;
    exec_tree->size++;
    list2(lex, exec_tree);
}

static struct ast *and_or(struct lexer *lex)
{
    return pipeline(lex);
}

static struct ast *pipeline(struct lexer *lex)
{
    return command(lex);
}

static struct ast *command(struct lexer *lex)
{
    struct ast *cmd = simple_command(lex);

    if (!cmd)
    {
        cmd = shell_command(lex);

        if (!cmd)
        {
            return error_handler(lex,"Error command: simple_command and\
                                 shell_command failed");
        }
    }

    return cmd;
}

static struct ast *simple_command(struct lexer *lex)
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

static void *simple_command2(struct lexer *lex, struct ast_cmd *cmd)
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

static struct ast *shell_command(struct lexer *lex)
{
    return rule_if(lex, 0); //change 0 to 1 dans change negate all the rest
}

// is_elif : 0 = if, 1 = elif
static struct ast *rule_if(struct lexer *lex, int is_elif)
{
    peek_token(lex);
    if (strcmp("if", lex->tok->data) != 0)
        return NULL;
    free_token(lex);

    struct ast_if *if_node = init_if();

    if_node->condition = compound_list(lex); // list or cmd
    if (!if_node->condition || lex->error == 2)
    {
        free_node(convert_node_ast(AST_IF, if_node));
        return error_handler(lex,
                             "Error rule_if: NO MATCHING PATERN after \"if\"");
    }

    next_token(lex);
    if (strcmp("then", lex->tok->data) != 0)
    {
        free_node(convert_node_ast(AST_IF, if_node));
        return error_handler(lex, "Error rule_if: \"then\" IS MISSING");
    }
    free_token(lex);

    if_node->then = compound_list(lex);
    if (!if_node->then || lex->error == 2)
    {
        free_node(convert_node_ast(AST_IF, if_node));
        return error_handler(
            lex, "Error rule_if: NO MATCHING PATERN after \"then\"");
    }

    if_node->else_body = else_clause(lex);

    if (!is_elif)
    {
        next_token(lex);
        if (lex->error == 2 || strcmp("fi", lex->tok->data) != 0)
        {
            free_node(convert_node_ast(AST_IF, if_node));
            return error_handler(lex, "Error rule_if: \"fi\" IS MISSING");
        }
        free_token(lex);
    }

    return convert_node_ast(AST_IF, if_node);
}

static struct ast *else_clause(struct lexer *lex)
{
    peek_token(lex);

    if (!strcmp(lex->tok->data, "else"))
    {
        free_token(lex);
        return compound_list(lex);
    }
    else if (!strcmp(lex->tok->data, "elif"))
    {
        free(lex->tok->data);
        lex->tok->data = strdup("if");
        return rule_if(lex, 1);
    }
    else
        return NULL;
}

static void new_line(struct lexer *lex)
{
    peek_token(lex);
    while (lex->tok->type == NEWLINE)
    {
        free_token(lex);
        peek_token(lex);
    }
}

static struct ast *compound_list(struct lexer *lex)
{
    new_line(lex); // will have stock the first token non new_line in lex

    struct ast *node = and_or(lex);
    if (!node)
        return error_handler(lex, "ERROR COMPOUND_LIST: NO MATCHIN PATTERN");

    struct ast_list *list = init_list(SIZE);

    compound_list2(lex, list);

    peek_token(lex);
    if (lex->tok->type == SEMICOLON)
        free_token(lex);
    
    new_line(lex);

    // multiple command
    if (list->size > 0)
        return convert_node_ast(AST_LIST, list);
    // only one command
    free(list->cmd_if);
    free(list);
    return node;
}

static void compound_list2(struct lexer *lex, struct ast_list *list)
{
    peek_token(lex);
    if (lex->tok->type != SEMICOLON && lex->tok->type != NEWLINE)
        return;

    if (lex->tok->type == NEWLINE)
    {
        new_line(lex);
        struct ast *node = and_or(lex);
        if (!node)
        {
            lex->error = 0;
            return;
        }
        list->cmd_if[list->size] = node;
        list->size++;
    }
    // case of SEMICOLON
    free_token(lex);

    compound_list2(lex, list);
}