#include "parser.h"

#include <stdlib.h>
#include <string.h>

#define SIZE 100

struct ast *input(struct lexer *lex);
static struct ast *list(struct lexer *lex);
static void list2(struct lexer *lex, struct ast_list *exec_tree);
struct ast *and_or(struct lexer *lex);
static struct ast *pipeline(struct lexer *lex);
static struct ast *command(struct lexer *lex);

/*
 * check if the current token is a shell command
 */
int is_shell_command(struct lexer *lex)
{
    char *data = lex->tok->data;
    return strcmp("if", data) == 0 || strcmp("else", data) == 0
        || strcmp("elif", data) == 0 || strcmp("then", data) == 0
        || strcmp("fi", data) == 0;
}

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
*Error handler puts an error in the "lex struct" and print the "error_message" only if the flag "print" is 1
*/
struct ast *error_handler(struct lexer *lex, int print, char *error_message)
{
    lex->error = 2;
    char var = error_message[0];
    var++;
    if (print)
        printf("%s\n", error_message);
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
        struct ast_cmd *cmd = init_ast(AST_CMD, 0);
        vector_append(cmd->arg, strdup("")); // faire la modif
        return convert_node_ast(AST_CMD, cmd);
    }

    struct ast *exec_tree = list(lex);
    if (lex->error == 2)
    {
        error_handler(lex, 1, "Error input: NO MATCHING PATERN");
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
    struct ast_list *exec_tree = init_ast(AST_LIST, SIZE);

    struct ast *head_cmd = and_or(lex);
    if (head_cmd != NULL)
        add_to_list(exec_tree, head_cmd);
    if (lex->error == 2)
    {
        error_handler(lex, 1, "Error input: NO MATCHING PATERN");
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

static void and_condition(struct lexer *lex, struct ast *parent, int child, struct ast *pipe)
{
    free_token(lex);
    struct ast_and *new = init_ast(AST_AND, 0);
    if (!child)
        new->left = pipe;
    else
        new->left = parent;
    
    new_lines(lex);
    struct ast *pipe2 = pipeline(lex);
    if (!pipe2)
    {
        free_node(convert_node_ast(AST_AND,new));
        free_node(pipe);
        error_handler(lex, 1, "Error and_or: inside && :NO MATCHING PATERN\n");
        return;
    }
    new->right = pipe2;
    parent = convert_node_ast(AST_AND,new);
    peek_token(lex);
}

static void or_condition(struct lexer *lex, struct ast *parent, int child, struct ast *pipe)
{
    free_token(lex);
    struct ast_or *new = init_ast(AST_OR, 0);
    
    if (!child)
        new->left = pipe;
    else
        new->left = parent;
    new_lines(lex);
    struct ast *pipe2 = pipeline(lex);
    if (!pipe2)
    {
        free_node(convert_node_ast(AST_OR,new));
        free_node(pipe);
        error_handler(lex, 1, "Error and_or: inside || : NO MATCHING PATERN\n");
        return;
    }
    new->right = pipe2;
    parent = convert_node_ast(AST_OR,new);
    peek_token(lex);
}

struct ast *and_or(struct lexer *lex)
{
    struct ast *pipe = pipeline(lex);
    
    if (!pipe)
        return NULL;

    struct ast *parent = NULL;
    int child = 0;

    peek_token(lex);

    if (lex->tok->type != WORD)
        return pipe;

    while (1)
    {
        if (!strcmp(lex->tok->data, "&&"))
            and_condition(lex, parent, child, pipe);

        else if (!strcmp(lex->tok->data, "||"))
            or_condition(lex, parent, child, pipe);

        else
            break;
        
        if (!parent)
            return NULL;
        child = 1;
    }

    if (!parent)
        free_node(pipe);
    return parent;
}

static struct ast *pipeline(struct lexer *lex)
{
    peek_token(lex);

    struct ast_not *not = NULL;

    if (lex->tok->type == WORD && !strcmp(lex->tok->data, "!"))
    {    
        free_token(lex);
        not = init_ast(AST_NOT, 0);
    }

    struct ast *cmd = command(lex);
    if (!cmd)
    {
        if (not)
        {
            lex->error = 2;
            free_node(convert_node_ast(AST_NOT, not));
        }
        return NULL;
    }

    peek_token(lex);

    struct ast *parent = NULL;

    if (not)
    {
        not->node = cmd;
        parent = convert_node_ast(AST_NOT, not);
    }
    else
        parent = cmd;

    while(lex->tok->type == WORD && !strcmp(lex->tok->data, "|"))
    {
        free_token(lex);

        new_lines(lex);

        struct ast *cmd2 = command(lex);

        if (!cmd2)
        {
            lex->error = 2;
            free_node(parent); //l'implémenter !!!!
            return NULL;
        }

        struct ast_pipe *pipe = init_ast(AST_PIPE, 0);

        pipe->left = parent;
        pipe->right = cmd2;
        parent = convert_node_ast(AST_PIPE, pipe);
    }

    return parent;
    //return command(lex);
}

static struct ast *command(struct lexer *lex)
{
    struct ast *cmd = simple_command(lex);

    if (!cmd)
    {
        struct ast *tmp_cmd = shell_command(lex);

        if (!tmp_cmd)
            return error_handler(lex, 0, "ERROR");
            //flag print is set to 0 to not print the error message

        struct ast_sh_cmd *sh_cmd = init_ast(AST_SH_CMD, 0);
        sh_cmd->cmd = tmp_cmd;
        struct ast *redir = redirection(lex);

        while (redir)
        {
            sh_cmd->redir[sh_cmd->size_redir] = redir;
            sh_cmd->size_redir++;
            redir = redirection(lex);
        }

        return convert_node_ast(AST_SH_CMD, sh_cmd);
    }

    return cmd;
}

struct ast *prefix(struct lexer *lex)
{
    peek_token(lex);

    struct ast_prefix *pref = init_ast(AST_PREFIX, 0);

    if(lex->tok->type == ASSIGNEMENT_WORD)
    {
        pref->assign_word = strdup(lex->tok->data);
        return convert_node_ast(AST_PREFIX,pref);
    }

    struct ast *redir = redirection(lex);

    if (!redir)
    {
        free_node(convert_node_ast(AST_PREFIX, pref));
        return NULL;
    }

    pref->redir = redir;

    return convert_node_ast(AST_PREFIX,pref);
}

struct ast *redirection(struct lexer *lex)
{
    peek_token(lex);

    int io_number; // 0 if no ionumber specified 1 otherwise

    struct ast_redir *redir = init_ast(AST_REDIR, 0);

    if (lex->tok->type == IO_NUMBER)
    {
        redir->io_number = strdup(lex->tok->data);
        free_token(lex);
        peek_token(lex);
        io_number = 1;
    }

    if (lex->tok->type != WORD)
    {
        if(!io_number)
            lex->error = 2;
        free_node(convert_node_ast(AST_REDIR, redir));
        return NULL;
    }

    char *data = lex->tok->data;

    if (!strcmp(">", data))
        redir->type = S_RIGHT;
    else if (!strcmp("<", data))
        redir->type = S_LEFT;
    else if (!strcmp(">>", data))
        redir->type = D_RIGHT;
    else if (!strcmp(">&", data))
        redir->type = RIGHT_AND;
    else if (!strcmp("<&", data))
        redir->type = LEFT_AND;
    else if (!strcmp(">|", data))
        redir->type = RIGHT_PIP; 
    else if (!strcmp("<>", data))
        redir->type = LEFT_RIGHT;
    else
    {
        if(!io_number)
            lex->error = 2;
        free_node(convert_node_ast(AST_REDIR, redir));
        return NULL;        
    } 

    free_token(lex);
    peek_token(lex);

    if (lex->tok->type != WORD)
    {
        if(io_number)
            lex->error = 2;
        free_node(convert_node_ast(AST_REDIR, redir));
        return NULL;
    }

    redir->exit_file = strdup(lex->tok->data);
    free_token(lex);

    return convert_node_ast(AST_REDIR, redir);
}
