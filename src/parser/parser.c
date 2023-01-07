#include "lexer.h"
#include "parser.h"
#include "token.h"

static struct lexer *lex = init_lexer();
static int error;



struct ast *convert_node_ast(enum ast_type type, void *node)
{
    struct ast *ast_node = malloc(sizeof(struct ast));
    ast_node->type = type;

    if (type == LIST)
        ast_node->data.ast_list = *((struct ast_list*)(node));
    if (type == CMD)
        ast_node->data.ast_cmd = *((struct ast_cmd*)(node));
    if (type == IF)
        ast_node->data.ast_if = *((struct ast_if*)(node));

    return ast_node;
}

void peek_token(struct lexer* lex)
{
    if(!lex->tok)
       next_token(lex);
}

struct ast *input(struct lexer* lex)
{
    peek_token(lex);
    if (lex->tok == EOF || lex->tok->type == NEWLINE) // EOF OU \n
    {
        if (lex->tok)
            free(lex->tok); // futur pop
        return NULL; // ast vide
    }
    
    struct ast *exec_tree = list(lex);
    
    if (lex->error == 2)
    {
        return NULL;
        //free ??
    }
    
    peek_token(lex);
    if (lex->tok == EOF || lex->tok->type == NEWLINE)
    {
        if (lex->tok)
            free(lex->tok);
        return exec_tree;
    }
    lex->error = 2;
    return NULL;
}

static struct ast *list(struct lexer* lex)
{
    int cpt_cmd = 0;
    struct ast_list *exec_tree = malloc(sizeof(struct ast_list));

    struct ast *head_cmd = and_or(lex);
    exec_tree->cmd = head_cmd;

    if (lex->error == 2)
        return NULL;
    cpt_cmd += 1;

    peek_token(lex);

    while(lex->tok->type == SEMICOLON)
    {
        free(lex->tok);

        struct ast *cmd = and_or(lex);
        
        exec_tree->cmd[cpt_cmd] = cmd;
        
        if (!cmd)
            break;
        
        cpt_cmd += 1;
  
        peek_token(lex);
    }

    if (cpt_cmd > 1) // plus d'une commande donc une list 
    {
        return convert_node_ast(LIST, exec_tree);
    }
    free(exec_tree);
    return head_cmd;
}

static struct ast and_or(struct lexer *lex)
{
    return pipeline(lex);
}

static struct ast pipeline(struct lexer *lex)
{
    return command(lex);
}

static struct ast command(struct lexer *lex)
{
    struct ast *cmd = simple_command(lex);
    
    if(!cmd)
    {
        cmd = shell_command(lex);
    
        if(!cmd)
        {
            lex->error = 2;
            return NULL;
        }
    }

    return cmd;
}

static struct ast simple_command(struct lexer *lex)
{
    peek_token(lex);

    if(lex->tok->type != WORD)
        return NULL;
    
    do{
        struct ast *node = element(lex);
    }while()
}

static struct ast element(struct lexer *lex)
{
    peek_token(lex);
    if (lex->tok->type == WORD)
    {
        
    }
}

static struct ast rule_if(void)
{
i   //TODO
}

static struct ast shell_command(struct lexer *lex)
{
    return rule_if(lex);
}

static struct ast else_clause(void)
{
    //TODO
}

static struct ast compound_list(void)
{
    //TODO
}
