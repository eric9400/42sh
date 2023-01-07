#include "lexer.h"
#include "parser.h"
#include "token.h"
#include "ast.h"

// static struct lexer *lex = NULL;
// static int error;

static struct ast *list(struct lexer* lex);

void pretty_print(struct ast *tree);
struct ast *convert_node_ast(enum ast_type type, void *node);
void peek_token(struct lexer* lex);
struct ast *input(FILE *filename);
static struct ast *list(struct lexer* lex);
static struct ast *and_or(struct lexer *lex);
static struct ast *pipeline(struct lexer *lex);
static struct ast *command(struct lexer *lex);
static struct ast *simple_command(struct lexer *lex);
static char *element(struct lexer *lex);
static struct ast *rule_if(struct lexer *lex);
static struct ast *shell_command(struct lexer *lex);
static struct ast *else_clause(struct lexer *lex);
static struct ast *compound_list(struct lexer *lex);

void pretty_print(struct ast *tree)
{
    if (!tree)
        printf("NULL");
    if (tree->type == AST_IF)
    {
        printf("if (");
        pretty_print(tree->data.ast_if.condition);
        printf(") then");
        pretty_print(tree->data.ast_if.then);
        printf("; else ");
        pretty_print(tree->data.ast_if.else_body);
        printf("fi");
    }
    else if (tree->type == AST_CMD)
    {
        printf("commande (");
        vector_print(tree->data.ast_cmd.arg);
        printf(")");
    }
    else if (tree->type == AST_LIST)
    {
        printf("list (");
        for (size_t i = 0; i < tree->data.ast_list.size; i++)
        {
            pretty_print(tree->data.ast_list.cmd_if[i]);
            printf(" ");
        }
        printf(")");
    }
}

struct ast *convert_node_ast(enum ast_type type, void *node)
{
    struct ast *ast_node = malloc(sizeof(struct ast));
    ast_node->type = type;

    if (type == AST_LIST)
        ast_node->data.ast_list = *((struct ast_list*)(node));
    if (type == AST_CMD)
        ast_node->data.ast_cmd = *((struct ast_cmd*)(node));
    if (type == AST_IF)
        ast_node->data.ast_if = *((struct ast_if*)(node));

    return ast_node;
}

void peek_token(struct lexer* lex)
{
    if(!lex->tok)
       next_token(lex);
}

struct ast *input(FILE *filename)
{
    struct lexer *lex = init_lexer(filename);
    peek_token(lex);
    if (lex->tok->type == END_OF_FILE || lex->tok->type == NEWLINE) // EOF OU \n
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
    if (lex->tok->type == END_OF_FILE || lex->tok->type == NEWLINE)
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
    struct ast_list *exec_tree = init_list();

    struct ast *head_cmd = and_or(lex);
    exec_tree->cmd_if[0] = head_cmd;

    if (lex->error == 2)
        return NULL;
    cpt_cmd += 1;

    peek_token(lex);

    while(lex->tok->type == SEMICOLON)
    {
        free(lex->tok);

        struct ast *cmd = and_or(lex);
        
        exec_tree->cmd_if[cpt_cmd] = cmd;
        
        if (!cmd)
            break;
        
        cpt_cmd += 1;
  
        peek_token(lex);
    }
    exec_tree->cmd_if[cpt_cmd] = NULL;

    if (cpt_cmd > 1) // plus d'une commande donc une list 
    {
        return convert_node_ast(AST_LIST, exec_tree);
    }
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
        {
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

    if(lex->tok->type != WORD)
    {
        free_node(convert_node_ast(AST_CMD, cmd));
        return NULL;
    }

    char *w = lex->tok->data;
    vector_append(cmd->arg,w);
    free(lex->tok);

    char *word;
    do{
        word = element(lex);
        vector_append(cmd->arg,word);
    }while(!word);

    return convert_node_ast(AST_CMD, cmd);
}

static char *element(struct lexer *lex)
{
    peek_token(lex);
    if (lex->tok->type == WORD)
    {
        char *word = strdup(lex->tok->data);
        free(lex->tok);
        return word;
    }
    return NULL;
}

static struct ast *rule_if(struct lexer *lex)
{
    peek_token(lex);

    if(lex->tok->type != IF)
    {
        return NULL;
    }

    free(lex->tok);

    struct ast_if *if_node = init_if();

    if_node->condition = compound_list(lex); // list or cmd
    
    if(!if_node->condition || lex->error == 2)
    {
        free_node(convert_node_ast(AST_IF, if_node));
        return NULL;
    }

    next_token(lex);

    if(lex->tok->type != THEN)
    {
        free_node(convert_node_ast(AST_IF, if_node));
        lex->error = 2;
        return NULL;
    }

    if_node->then = compound_list(lex);

    if(!if_node->then || lex->error == 2)
    {
        free_node(convert_node_ast(AST_IF, if_node));
        lex->error = 2;
        return NULL;
    }

    if_node->else_body = else_clause(lex);

    next_token(lex);

    if(lex->tok->type != FI)
    {
        free_node(convert_node_ast(AST_IF, if_node));
        lex->error = 2;
    }

    free(lex->tok);
    return convert_node_ast(AST_IF, if_node);
}

static struct ast *shell_command(struct lexer *lex)
{
    return rule_if(lex);
}

static struct ast *else_clause(struct lexer *lex)
{
    peek_token(lex);

    if(lex->tok->type != ELSE && lex->tok->type != ELIF)
        return NULL;

    if (lex->tok->type == ELSE)
    {
        free(lex->tok);
        return compound_list(lex);
    }

    //ELSE IF CASE :
    lex->tok = IF;
    return rule_if(lex);
}

static struct ast *compound_list(struct lexer *lex)
{
    peek_token(lex);

    while(lex->tok->type == NEWLINE)
    {
        free(lex->tok);
        peek_token(lex);
    }

    struct ast *node = and_or(lex);
    if(!node)
    {
        lex->error = 2;
        return NULL;
    }

    free(lex->tok);

    peek_token(lex);

    if(lex->tok->type != SEMICOLON && lex->tok->type != NEWLINE)
        return node;

    //TODO : continuer la fonction
    return NULL;
}
