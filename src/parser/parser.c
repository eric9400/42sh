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
        struct ast_cmd *cmd = init_cmd();
        vector_append(cmd->arg, strdup("")); // faire la modif
        return convert_node_ast(AST_CMD, cmd);
    }

    struct ast *exec_tree = list(lex);
    if (lex->error == 2)
    {
        error_handler(lex, 1, "Error input: NO MATCHING PATERN\n");
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
    struct ast_list *exec_tree = init_list(SIZE);

    struct ast *head_cmd = and_or(lex);
    if (head_cmd != NULL)
        add_to_list(exec_tree, head_cmd);
    if (lex->error == 2)
    {
        error_handler(lex, 1, "Error input: NO MATCHING PATERN\n");
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

/*struct ast *and_or_loop(struct lexer *lex)
{
    peek_token(lex);

    if (lex->tok->type != WORD)
        return NULL;
    
    if (!strcmp(lex->tok->data, "&&"))
    {
        free_token(lex);
        struct ast_and *new = init_and();
        new_lines(lex);
        struct ast *pipe2 = pipeline(lex);
        if (!pipe2)
        {
            free_ast(new);
            return error_handler(lex, 1, "Error and_or_loop: NO MATCHING PATERN\n");
        }
        new->right = pipe2;
        struct ast *res = and_or_loop(lex);
    }
    else if (!strcmp(lex->tok->data, "||"))
    {    
        free_token(lex);
        struct ast_or *new = init_or();
        new->left = pipe;
        new_lines(lex);
        struct ast *pipe2 = pipeline(lex);
        if (!pipe2)
        {
            free_ast(new);
            free_ast(pipe);
            return error_handler(lex, 1, "Error and_or: NO MATCHING PATERN\n");
        }
        new->right = pipe2;
    }
}*/

static void free_ast_tree(struct ast *tree)
{
    while(tree)
    {
        struct ast *tmp = NULL;
        if (tree->type == AST_AND)
        {
            tmp = tree->data->ast_and->left;
            if (tree->data->ast_and->right)
                free_node(tree->data->ast_and->right);
        }
        else if (tree->type == AST_OR)
        {
            tmp = tree->data->ast_or->left;
            if (tree->data->ast_or->right)
                free_node(tree->data->ast_or->right);
        }
        else
        {
            free_node(tree);
            return;
        }
        if (tree)
            free_node(tree);
        tree = tmp;
    }
}

static void and_condition(struct lexer *lex, struct ast *parent, int child, struct ast *pipe)
{
    free_token(lex);
    struct ast_and *new = init_and();
    if (!child)
        new->left = pipe;
    else
        new->left = parent;
    
    new_lines(lex);
    struct ast *pipe2 = pipeline(lex);
    if (!pipe2)
    {
        free_ast_tree(convert_node_ast(AST_AND,new));
        free_node(pipe);
        return error_handler(lex, 1, "Error and_or: inside && :NO MATCHING PATERN\n");
    }
    new->right = pipe2;
    parent = convert_node_ast(AST_AND,new);
    peek_token(lex);
}

static void or_condition(struct lexer *lex, struct ast *parent, int child, struct ast *pipe)
{
    free_token(lex);
    struct ast_or *new = init_or();
    
    if (!child)
        new->left = pipe;
    else
        new->left = parent;
    new_lines(lex);
    struct ast *pipe2 = pipeline(lex);
    if (!pipe2)
    {
        free_ast_tree(convert_node_ast(AST_OR,new));
        free_node(pipe);
        return error_handler(lex, 1, "Error and_or: inside || : NO MATCHING PATERN\n");
    }
    new->right = pipe2;
    parent = convert_node_ast(AST_OR,new);
    peek_token(lex);
}

struct ast *and_or(struct lexer *lex)
{
    struct ast *pipe = pipeline(lex);
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
        
        child = 1;
    }

    if (!parent)
        free_node(pipe);
    return parent;
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
        struct ast_sh_cmd *sh_cmd = shell_command(lex);

        if (!sh_cmd)
            return error_handler(lex, 0, "ERROR");
            //flag print is set to 0 to not print the error message

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

struct ast_prefix *prefix(struct lexer *lex)
{
    peek_token(lex);

    struct ast_prefix *prefix = init_prefix();

    if(lex->tok->type == ASSIGNEMENT_WORD)
    {
        prefix->assign_word = strdup(lex->tok->data);
        return convert_node_ast(AST_PREFIX,prefix);
    }

    struct ast *redir = redirection(lex);

    if (!redir)
    {
        free_node(convert_node_ast(AST_PREFIX, prefix));
        return NULL;
    }

    prefix->redir = redir;

    return convert_node_ast(AST_PREFIX,prefix);
}

static struct ast *redirection(struct lexer *lex)
{
    peek_token(lex);

    int io_number; // 0 if no ionumber specified 1 otherwise

    struct ast_redir *redir = init_redir();

    if (lex->tok->type == IO_NUMBER)
    {
        redir->io_number = strdup(lex->tok->data);
        free_token(lex);
        peek_token(lex);
        io_number = 1;
    }

    if (lex->tok->type != WORD)
    {
        if(io_number)
            lex->error = 2;
        return NULL;
    }

    char *redir = lex->tok->data;

    if (strcmp(">", redir))
        redir->type = S_RIGHT;
    else if (strcmp("<", redir))
        redir->type = S_LEFT;
    else if (strcmp(">>", redir))
        redir->type = D_RIGHT;
    else if (strcmp(">&", redir))
        redir->type = RIGHT_AND;
    else if (strcmp("<&", redir))
        redir->type = LEFT_AND;
    else if (strcmp(">|", redir))
        redir->type = RIGHT_PIP; 
    else if (strcmp("<>", redir))
        redir->type = LEFT_RIGHT;
    else
    {
        if(io_number)
            lex->error = 2;
        free_node(redir);
        return NULL;        
    } 

    free_token(lex);
    peek_token(lex);

    if (lex->tok->type != WORD)
    {
        if(io_number)
            lex->error = 2;
        free_node(redir);
        return NULL;
    }

    redir->exit_file = strdup(lex->tok->data);
    free_token(lex);

    return convert_node_ast(AST_REDIR, redir);
}

static struct ast *rule_while(struct lexer *lex)
{
    peek_token(lex);
    if (strcmp("while", lex->tok->data) != 0)
        return NULL;
    free_token(lex);

    struct ast_while *while_node = init_while();

    while_node->condition = compound_list(lex);
    if (!while_node->condition || lex->error == 2)
    {
        free_node(convert_node_ast(AST_WHILE, while_node));
        return error_handler(lex, 1,
                    "Error rule_while: NO MATCHING PATERN after \"while\"");
    }

    next_token(lex);
    if (strcmp("do", lex->tok->data) != 0)
    {
        free_node(convert_node_ast(AST_WHILE, while_node));
        return error_handler(lex, 1, "Error rule_while: \"do\" IS MISSING");
    }
    free_token(lex);

    while_node->while_body = compound_list(lex);
    if (!while_node->while_body || lex->error == 2)
    {
        free_node(convert_node_ast(AST_WHILE, while_node));
        return error_handler(
            lex, 1, "Error rule_while: NO MATCHING PATERN after \"do\"");
    }

    next_token(lex);
    if (lex->error == 2 || strcmp("done", lex->tok->data) != 0)
    {
        free_node(convert_node_ast(AST_WHILE, while_node));
        return error_handler(lex, 1, "Error rule_while: \"done\" IS MISSING");
    }
    free_token(lex);

    return convert_node_ast(AST_WHILE, while_node);
}

static struct ast *rule_until(struct lexer *lex)
{
    peek_token(lex);
    if (strcmp("until", lex->tok->data) != 0)
        return NULL;
    free_token(lex);

    struct ast_until *until_node = init_until();

    until_node->condition = compound_list(lex);
    if (!until_node->condition || lex->error == 2)
    {
        free_node(convert_node_ast(AST_UNTIL, until_node));
        return error_handler(lex, 1,
                    "Error rule_until: NO MATCHING PATERN after \"until\"");
    }

    next_token(lex);
    if (strcmp("do", lex->tok->data) != 0)
    {
        free_node(convert_node_ast(AST_UNTIL, until_node));
        return error_handler(lex, 1, "Error rule_until: \"do\" IS MISSING");
    }
    free_token(lex);

    until_node->until_body = compound_list(lex);
    if (!until_node->until_body || lex->error == 2)
    {
        free_node(convert_node_ast(AST_UNTIL, until_node));
        return error_handler(
            lex, 1, "Error rule_until: NO MATCHING PATERN after \"do\"");
    }

    next_token(lex);
    if (lex->error == 2 || strcmp("done", lex->tok->data) != 0)
    {
        free_node(convert_node_ast(AST_UNTIL, until_node));
        return error_handler(lex, 1, "Error rule_until: \"done\" IS MISSING");
    }
    free_token(lex);

    return convert_node_ast(AST_UNTIL, until_node);
}

static void *rule_for_error(struct ast_for *for_node, struct lexer *lex)
{
    free_node(convert_node_ast(AST_FOR, for_node));
    lex->error = 2;
    return NULL;
}

static void free_peek(struct lexer *lex)
{
    free_token(lex);
    peek_token(lex);
    return;
}

struct ast *rule_for(struct lexer *lex)
{
    peek_token(lex);

    if (lex->tok->type != WORD || !strcmp("for", lex->tok->data))
        return NULL;

    struct ast_for *for_node = init_for();

    free_peek(lex);

    if (lex->tok->type != WORD)
        return rule_for_error(for_node, lex);

    for_node->var = strdup(lex->tok->data);
    free_peek(lex);

    if (lex->tok->type == SEMICOLON)
        free_peek(lex);
    else if (lex->tok->type == NEWLINE)
    {
        free_peek(lex);

        new_lines(lex);

        if (lex->tok->type != WORD)
            return rule_for_error(for_node, lex);

        if (strcmp("do", lex->tok->data))
        {

            if (strcmp("in", lex->tok->data))
                return rule_for_error(for_node, lex);

            free_peek(lex);

            while(lex->tok->type == WORD)
            {
                vector_append(for_node->arg, lex->tok->data);
                free_peek(lex);
            }
            vector_append(for_node->arg, NULL);

            if (lex->tok->type != SEMICOLON)
                return rule_for_error(for_node, lex);

            free_token(lex);
        }
    }

    next_token(lex);

    if (lex->tok->type != WORD || strcmp("do", lex->tok->data))
        return rule_for_error(for_node, lex);

    free_token(lex);

    for_node->for_list = compound_list(lex);

    if (lex->error == 2)
    {
        free_node(convert_node_ast(AST_FOR, for_node));
        return NULL;
    }

    next_token(lex);

    if(strcmp("done", lex->tok->data))
        return rule_for_error(for_node, lex);

    free_token(lex);

    return convert_node_ast(AST_FOR,for_node);
}