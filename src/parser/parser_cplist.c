#include "parser_cplist.h"

#include <stdlib.h>
#include <string.h>

struct ast *compound_list(struct lexer *lex);
static void compound_list2(struct lexer *lex, struct ast_list *list);
struct ast *compound_list_spe(struct lexer *lex);
static void compound_list_spe2(struct lexer *lex, struct ast_list *list);

void free_peek(struct lexer *lex)
{
    free_token(lex);
    peek_token(lex);
    return;
}

struct ast *compound_list(struct lexer *lex)
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

struct ast *compound_list_spe(struct lexer *lex)
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