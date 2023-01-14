#include "parser.h"

#include <stdlib.h>
#include <string.h>

#define SIZE 100

struct ast *shell_command(struct lexer *lex);
static struct ast *rule_if(struct lexer *lex, int opt);
static struct ast *else_clause(struct lexer *lex);
static struct ast *compound_list(struct lexer *lex);
static void compound_list2(struct lexer *lex, struct ast_list *list);

struct ast *shell_command(struct lexer *lex)
{
    return rule_if(lex, 1);
}

// is_if : 1 = if, 0 = elif
static struct ast *rule_if(struct lexer *lex, int is_if)
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
        return error_handler(lex, 1,
                             "Error rule_if: NO MATCHING PATERN after \"if\"");
    }

    next_token(lex);
    if (strcmp("then", lex->tok->data) != 0)
    {
        free_node(convert_node_ast(AST_IF, if_node));
        return error_handler(lex, 1, "Error rule_if: \"then\" IS MISSING");
    }
    free_token(lex);

    if_node->then = compound_list(lex);
    if (!if_node->then || lex->error == 2)
    {
        free_node(convert_node_ast(AST_IF, if_node));
        return error_handler(
            lex, 1, "Error rule_if: NO MATCHING PATERN after \"then\"");
    }

    if_node->else_body = else_clause(lex);

    if (is_if)
    {
        next_token(lex);
        if (lex->error == 2 || strcmp("fi", lex->tok->data) != 0)
        {
            free_node(convert_node_ast(AST_IF, if_node));
            return error_handler(lex, 1, "Error rule_if: \"fi\" IS MISSING");
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
        return rule_if(lex, 0);
    }
    else
        return NULL;
}

static struct ast *compound_list(struct lexer *lex)
{
    new_lines(lex); // will have stock the first token non new_line in lex

    struct ast_list *list = init_list(SIZE);
    struct ast *node = and_or(lex);
    if (!node)
    {
        free_node(convert_node_ast(AST_LIST, list));
        return error_handler(lex, 1, "ERROR COMPOUND_LIST: NO MATCHIN PATTERN");
    }

    add_to_list(list, node);

    compound_list2(lex, list);

    peek_token(lex);
    if (lex->tok->type == SEMICOLON)
        free_token(lex);
    
    new_lines(lex);

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
    free_token(lex);

    new_lines(lex);
    struct ast *node = and_or(lex);
    if (!node)
    {
        lex->error = 0;
        return;
    }
    add_to_list(list, node);

    compound_list2(lex, list);
}
