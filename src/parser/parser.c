#include "parser.h"

#include <stdlib.h>
#include <string.h>

// static struct lexer *lex = NULL;
// static int error;

static struct ast *list(struct lexer *lex);
struct ast *input(struct lexer *lex);
static struct ast *list(struct lexer* lex);
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
    if(!lex->tok)
       next_token(lex);
}

static struct ast *error_handler(struct lexer *lex, char *error_message)
{
    lex->error = 2;
    printf("%s\n", error_message);
    return NULL;
}

struct ast *input(struct lexer *lex)
{
    peek_token(lex);
    if (lex->tok->type == END_OF_FILE || lex->tok->type == NEWLINE) // EOF OU \n
    {
        free_token(lex);
        return NULL; // ast vide
    }
    
    struct ast *exec_tree = list(lex);
    if (lex->error == 2)
    {
        printf("Error input: NO MATCHING PATERN\n");
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
    struct ast_list *exec_tree = init_list(10);

    struct ast *head_cmd = and_or(lex);
    if (head_cmd != NULL)
    {
        exec_tree->cmd_if[cpt_cmd] = head_cmd;
        exec_tree->size++;
        cpt_cmd++;
    }
    if (lex->error == 2)
    {
        printf("Error list: NO MATCHING PATERN\n");
        return convert_node_ast(AST_LIST, exec_tree);
    }

    peek_token(lex);
    while(lex->tok->type == SEMICOLON)
    {
        free_token(lex);
        struct ast *cmd = and_or(lex);
        if (!cmd)
        {
            lex->error = 0;
            break;
        }

        exec_tree->cmd_if[cpt_cmd] = cmd;
        exec_tree->size++; 
        cpt_cmd++;
  
        peek_token(lex);
    }

    // plusieurs commandes donc une list 
    if (cpt_cmd > 1)
        return convert_node_ast(AST_LIST, exec_tree);

    // une seule commande
    free(exec_tree->cmd_if);
    free(exec_tree);
    return head_cmd;
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
    
    if(!cmd)
    {
        cmd = shell_command(lex);
    
        if(!cmd)
        {    //return error_handler(lex,"Error command: simple_command and shell_command failed");
            lex->error = 2;
            return NULL;        
        }
    }

    return cmd;
}

static struct ast *simple_command(struct lexer *lex)
{
    struct ast_cmd *cmd = init_cmd();
    
    peek_token(lex);
    if(lex->tok->type != WORD && lex->tok->type != S_QUOTE)
    {
        free_node(convert_node_ast(AST_CMD, cmd));
        return NULL;
    }

    char *word = strdup(lex->tok->data);
    vector_append(cmd->arg, word);
    free_token(lex);

    do{
        word = element(lex);
        vector_append(cmd->arg, word);
    } while(word != NULL);

    return convert_node_ast(AST_CMD, cmd);
}

static char *element(struct lexer *lex)
{
    peek_token(lex);
    if (lex->tok->type == WORD || lex->tok->type == S_QUOTE)
    {
        char *word = strdup(lex->tok->data);
        free_token(lex);
        return word;
    }
    return NULL;
}

static struct ast *shell_command(struct lexer *lex)
{
    return rule_if(lex, 0);
}

//opt : 0 = if, 1 = elif
static struct ast *rule_if(struct lexer *lex, int opt)
{
    peek_token(lex);
    if(lex->tok->type != IF)
        return NULL;
    free_token(lex);

    struct ast_if *if_node = init_if();

    if_node->condition = compound_list(lex); // list or cmd
    if(!if_node->condition || lex->error == 2)
    {
        free_node(convert_node_ast(AST_IF, if_node));
        return error_handler(lex, "Error rule_if: NO MATCHING PATERN after \"if\"");
    }

    next_token(lex);
    if(lex->tok->type != THEN)
    {
        free_node(convert_node_ast(AST_IF, if_node));
        return error_handler(lex, "Error rule_if: \"then\" IS MISSING");
    }
    free_token(lex);

    if_node->then = compound_list(lex);
    if(!if_node->then || lex->error == 2)
    {
        free_node(convert_node_ast(AST_IF, if_node));
        return error_handler(lex, "Error rule_if: NO MATCHING PATERN after \"then\"");
    }

    if_node->else_body = else_clause(lex);

    if(!opt)
    {
        next_token(lex);
        if(lex->tok->type != FI || lex->error == 2)
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
    if(lex->tok->type != ELSE && lex->tok->type != ELIF)
        return NULL;

    if (lex->tok->type == ELSE)
    {
        free_token(lex);
        return compound_list(lex);
    }

    //ELSE IF CASE
    lex->tok->type = IF;
    return rule_if(lex,1);
}

static struct ast *compound_list(struct lexer *lex)
{
    //just pour tester le reste
    /*peek_token(lex);
    if (strcmp(lex->tok->data, "true") != 0
        && strcmp(lex->tok->data, "false") != 0)
        error_handler(lex, "Error compound_list: NO MATCHING PATERN");
    struct ast_cmd *cmd = init_cmd();
    vector_append(cmd->arg, strdup(lex->tok->data));
    free_token(lex);
    return convert_node_ast(AST_CMD, cmd);*/
   
    peek_token(lex);

    while(lex->tok->type == NEWLINE)
    {
        free_token(lex);
        peek_token(lex);
    }

    struct ast *node = and_or(lex);
    if(!node)
    {
        lex->error = 2;
        return NULL;
    }

    peek_token(lex);

    if(lex->tok->type != SEMICOLON && lex->tok->type != NEWLINE)
        return node;

    int last_token;
    int cpt_cmd = 0;

    struct ast_list *list = init_list(10);

    while(lex->tok->type == SEMICOLON || lex->tok->type == NEWLINE)
    {
        last_token = 0; // 0 = ';' | 1 = '\n'

        while(lex->tok->type == SEMICOLON || lex->tok->type == NEWLINE)
        {
            if(lex->tok->type == SEMICOLON && last_token == 1)
            {
                free_node(node);
                free_node(convert_node_ast(AST_LIST,list));
                return error_handler(lex,"ERROR COMPOUND_LIST : wrong token\n");
            }
            free_token(lex);
            peek_token(lex);
            last_token = (lex->tok->type == SEMICOLON);
        }

        struct ast *res_cmd = and_or(lex);
        if(!res_cmd)
        {
            lex->error = 0;
            break;
        }

        if(cpt_cmd == 0)
        {
            list->cmd_if[cpt_cmd] = node;
            list->size++;
            cpt_cmd += 1;
        }
        list->cmd_if[cpt_cmd] = res_cmd;
        list->size++;
        
        cpt_cmd += 1;

        peek_token(lex);
    }

    // multiple command 
    if(cpt_cmd > 0)
        return convert_node_ast(AST_LIST,list);
    // only one command 
    free_node(convert_node_ast(AST_LIST,list));
    return node;
}
