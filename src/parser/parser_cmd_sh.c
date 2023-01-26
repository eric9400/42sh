#include "parser_cmd_sh.h"

#include <stdlib.h>
#include <string.h>

#define SIZE 150

struct ast *shell_command(struct lexer *lex);
static struct ast *rule_if(struct lexer *lex, int opt);
static struct ast *else_clause(struct lexer *lex);
static struct ast *rule_while(struct lexer *lex);
static struct ast *rule_until(struct lexer *lex);
static struct ast *rule_for(struct lexer *lex);
static int rule_for_in(struct lexer *lex, struct ast_for *for_node);
static struct ast *compound_list(struct lexer *lex);
static void compound_list2(struct lexer *lex, struct ast_list *list);
static struct ast *compound_list_spe(struct lexer *lex);
static void compound_list_spe2(struct lexer *lex, struct ast_list *list);

static void free_peek(struct lexer *lex)
{
    free_token(lex);
    peek_token(lex);
    return;
}

// 37 lines
struct ast *shell_command(struct lexer *lex)
{
    peek_token(lex);
    if (lex->tok->type == OPERATOR && strcmp(lex->tok->data, "(") == 0)
    {
        free_peek(lex);
        struct ast_subshell *ast_sub = init_ast(AST_SUBSHELL);
        ast_sub->sub = compound_list(lex);
        peek_token(lex);
        if (!ast_sub->sub || lex->error == 2 || lex->tok->type != OPERATOR
            || strcmp(lex->tok->data, ")") != 0)
        {
            free_node(convert_node_ast(AST_SUBSHELL, ast_sub));
            return error_handler(lex, 1,
                                 "ERROR SUBSHELL: INVALID COMPOUND LIST");
        }
        free_token(lex);
        return convert_node_ast(AST_SUBSHELL, ast_sub);
    }

    if (lex->tok->type == OPERATOR && strcmp(lex->tok->data, "{") == 0)
    {
        free_peek(lex);
        struct ast *comp_list = compound_list_spe(lex);
        peek_token(lex);
        if (lex->error == 2 || lex->tok->type != OPERATOR
            || strcmp(lex->tok->data, "}") != 0)
        {
            free_node(comp_list);
            return error_handler(lex, 1,
                                 "ERROR BLOCKCOMMAND: INVALID COMPOUND LIST");
        }
        free_token(lex);
        return comp_list;
    }

    struct ast *cmd_if = rule_if(lex, 1);

    if (cmd_if)
        return cmd_if;

    struct ast *cmd_while = rule_while(lex);

    if (cmd_while)
        return cmd_while;

    struct ast *cmd_until = rule_until(lex);

    if (cmd_until)
        return cmd_until;

    struct ast *cmd_for = rule_for(lex);

    if (cmd_for)
        return cmd_for;

    return NULL;
}

// 28 lines
// is_if : 1 = if, 0 = elif
static struct ast *rule_if(struct lexer *lex, int is_if)
{
    peek_token(lex);
    if (strcmp("if", lex->tok->data) != 0)
        return NULL;
    free_token(lex);

    struct ast_if *if_node = init_ast(AST_IF);

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

// 26 lines
static struct ast *rule_while(struct lexer *lex)
{
    peek_token(lex);
    if (strcmp("while", lex->tok->data) != 0)
        return NULL;
    free_token(lex);

    struct ast_while *while_node = init_ast(AST_WHILE);

    while_node->condition = compound_list(lex);
    if (!while_node->condition || lex->error == 2)
    {
        free_node(convert_node_ast(AST_WHILE, while_node));
        return error_handler(
            lex, 1, "Error rule_while: NO MATCHING PATERN after \"while\"");
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

// 26 lines
static struct ast *rule_until(struct lexer *lex)
{
    peek_token(lex);
    if (strcmp("until", lex->tok->data) != 0)
        return NULL;
    free_token(lex);

    struct ast_until *until_node = init_ast(AST_UNTIL);

    until_node->condition = compound_list(lex);
    if (!until_node->condition || lex->error == 2)
    {
        free_node(convert_node_ast(AST_UNTIL, until_node));
        return error_handler(
            lex, 1, "Error rule_until: NO MATCHING PATERN after \"until\"");
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
    return error_handler(lex, 1, "ERROR FOR: INVALID MATCHING PATERN");
}

static int is_name(char *str, size_t size)
{
    if (str[0] != '_' && (str[0] < 'a' || str[0] > 'z')
        && (str[0] < 'A' || str[0] > 'Z'))
        return 0;
    for (size_t i = 1; i < size; i++)
    {
        if (str[0] != '_' && (str[0] < 'a' || str[0] > 'z')
            && (str[0] < 'A' || str[0] > 'Z') && (str[0] < '0' || str[0] > '9'))
            return 0;
    }
    return 1;
}

// 32 lines
struct ast *rule_for(struct lexer *lex)
{
    peek_token(lex);

    if ((lex->tok->type != WORD && lex->tok->type != ASSIGNMENT_WORD)
        || strcmp("for", lex->tok->data))
        return NULL;

    struct ast_for *for_node = init_ast(AST_FOR);

    free_peek(lex);

    if ((lex->tok->type != WORD && lex->tok->type != ASSIGNMENT_WORD)
        || !is_name(lex->tok->data, strlen(lex->tok->data)))
        return rule_for_error(for_node, lex);

    for_node->var = strdup(lex->tok->data);
    free_peek(lex);

    if (lex->tok->type == SEMICOLON)
        free_peek(lex);
    else
    {
        if (rule_for_in(lex, for_node))
            return rule_for_error(for_node, lex);
    }

    next_token(lex);

    if (lex->tok->type == NEWLINE)
        new_lines(lex);

    if ((lex->tok->type != WORD && lex->tok->type != ASSIGNMENT_WORD)
        || strcmp("do", lex->tok->data))
        return rule_for_error(for_node, lex);

    free_token(lex);

    for_node->for_list = compound_list(lex);

    if (lex->error == 2)
    {
        free_node(convert_node_ast(AST_FOR, for_node));
        return NULL;
    }

    next_token(lex);

    if (strcmp("done", lex->tok->data))
        return rule_for_error(for_node, lex);

    free_token(lex);

    return convert_node_ast(AST_FOR, for_node);
}

// 14 lines
static int rule_for_in(struct lexer *lex, struct ast_for *for_node)
{
    new_lines(lex);

    if (lex->tok->type != WORD && lex->tok->type != ASSIGNMENT_WORD)
        return 2;

    if (strcmp("do", lex->tok->data))
    {
        if (strcmp("in", lex->tok->data))
            return 2;

        free_peek(lex);

        while (lex->tok->type == WORD || lex->tok->type == ASSIGNMENT_WORD)
        {
            vector_append(for_node->arg, strdup(lex->tok->data));
            free_peek(lex);
        }
        // vector_append(for_node->arg, NULL);

        if (lex->tok->type != SEMICOLON && lex->tok->type != NEWLINE)
            return 2;

        free_token(lex);
    }

    return 0;
}

// 17 lines
static struct ast *compound_list(struct lexer *lex)
{
    new_lines(lex); // will have stock the first token non new_line in lex

    struct ast_list *list = init_ast(AST_LIST);
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

// 15 lines
static void compound_list2(struct lexer *lex, struct ast_list *list)
{
    peek_token(lex);
    if (lex->tok->type != SEMICOLON && lex->tok->type != NEWLINE)
        return;
    free_token(lex);

    new_lines(lex);
    if (lex->tok->type == SEMICOLON)
    {
        error_handler(
            lex, 1, "ERROR: COMPOUND_LIST2: INVALID AND_OR AFTER VALID \";\"");
        return;
    }
    struct ast *node = and_or(lex);
    if (!node)
    {
        // error_handler(lex, 1, "ERROR: COMPOUND_LIST2: INVALID AND_OR AFTER
        // VALID \";\"");
        lex->error = 0;
        return;
    }
    add_to_list(list, node);

    compound_list2(lex, list);
}

static struct ast *compound_list_spe(struct lexer *lex)
{
    new_lines(lex); // will have stock the first token non new_line in lex

    struct ast_list *list = init_ast(AST_LIST);
    struct ast *node = and_or(lex);
    if (!node)
    {
        free_node(convert_node_ast(AST_LIST, list));
        return error_handler(lex, 1, "ERROR COMPOUND_LIST SPE");
    }
    add_to_list(list, node);

    new_lines(lex);

    peek_token(lex);
    if (lex->tok->type != SEMICOLON && lex->tok->type != NEWLINE && node->type != AST_LIST)
    {
        free_node(convert_node_ast(AST_LIST, list));
        return error_handler(lex, 1, "ERROR COMPOUND_LIST SPE: ; or \\n");
    }
    if (lex->tok->type == SEMICOLON || lex->tok->type == NEWLINE)
        free_token(lex);

    compound_list_spe2(lex, list);
    if (lex->error != 0)
    {
        free_node(convert_node_ast(AST_LIST, list));
        return error_handler(lex, 1, "ERROR COMPOUND_LIST SPE");
    }

    new_lines(lex);

    // multiple command
    if (list->size > 0)
        return convert_node_ast(AST_LIST, list);
    // only one command
    free(list->cmd_if);
    free(list);
    return node;
}

static void compound_list_spe2(struct lexer *lex, struct ast_list *list)
{
    new_lines(lex);
    if (lex->tok->type == SEMICOLON)
    {
        error_handler(lex, 1, "ERROR: COMPOUND_LIST SPE 2.1");
        return;
    }
    struct ast *node = and_or(lex);
    if (!node)
    {
        lex->error = 0;
        return;
    }
    add_to_list(list, node);
    
    peek_token(lex);
    if (lex->tok->type != SEMICOLON && lex->tok->type != NEWLINE && node->type != AST_LIST)
    {
        error_handler(lex, 1, "ERROR COMPOUND_LIST SPE 2.2");
        return;
    }
    if (lex->tok->type == SEMICOLON || lex->tok->type == NEWLINE)
        free_token(lex);
    compound_list_spe2(lex, list);
}