#include "parser_cplist.h"

#include <stdlib.h>
#include <string.h>

struct ast *rule_case(struct lexer *lex);
static void case_clause(struct lexer *lex, struct ast_case *ast_case);
static void case_clause2(struct lexer *lex, struct ast_case *ast_case);
static struct ast *case_item(struct lexer *lex);
static void case_item2(struct lexer *lex, struct ast_case_item *item);
struct ast *compound_list(struct lexer *lex);
static void compound_list2(struct lexer *lex, struct ast_list *list);

void free_peek(struct lexer *lex)
{
    free_token(lex);
    peek_token(lex);
    return;
}

struct ast *rule_case(struct lexer *lex)
{
    peek_token(lex);
    if (strcmp(lex->tok->data, "case") != 0)
        return NULL;
    free_token(lex);

    struct ast_case *ast_case = init_ast(AST_CASE);

    peek_token(lex);
    if (lex->tok->type != WORD && lex->tok->type != ASSIGNMENT_WORD)
    {
        free_node(convert_node_ast(AST_CASE, ast_case));
        return error_handler(lex, 1, "ERROR CASE: INVALID VALUE");
    }
    ast_case->value = strdup(lex->tok->data);
    free_token(lex);

    new_lines(lex);
    if (strcmp(lex->tok->data, "in") != 0)
    {
        free_node(convert_node_ast(AST_CASE, ast_case));
        return error_handler(lex, 1, "ERROR CASE: LACK OF \"in\"");
    }
    free_token(lex);

    new_lines(lex);

    case_clause(lex, ast_case);
    if (lex->error != 0)
    {
        free_node(convert_node_ast(AST_CASE, ast_case));
        return error_handler(lex, 1, "ERROR CASE: BAD CASE CLAUSE");
    }

    peek_token(lex);
    if (strcmp(lex->tok->data, "esac") != 0)
    {
        free_node(convert_node_ast(AST_CASE, ast_case));
        return error_handler(lex, 1, "ERROR CASE: LACK OF \"esac\"");
    }
    free_token(lex);

    return convert_node_ast(AST_CASE, ast_case);
}

static void case_clause(struct lexer *lex, struct ast_case *ast_case)
{
    struct ast *item = case_item(lex);
    if (lex->error != 0)
    {
        free_node(item);
        error_handler(lex, 1, "ERROR CASE ITEM: BAD ITEM");
        return;
    }
    if (!item)
        return;

    add_to_list(ast_case->items, item);

    case_clause2(lex, ast_case);
    if (lex->error != 0)
    {
        error_handler(lex, 1, "ERROR CASE ITEM: BAD ITEM PLUS");
        return;
    }
    if (!item)
        return;

    new_lines(lex);
}

static void case_clause2(struct lexer *lex, struct ast_case *ast_case)
{
    peek_token(lex);
    peek_token(lex);
    if (lex->tok->type != SEMICOLON || lex->tok2->type != SEMICOLON)
        return;
    free_token(lex);
    free_token(lex);

    new_lines(lex);
    if (strcmp(lex->tok->data, "esac") == 0)
        return;

    struct ast *item = case_item(lex);
    if (!item)
    {
        lex->error = 0;
        return;
    }
    add_to_list(ast_case->items, item);

    case_clause2(lex, ast_case);
}

static struct ast *case_item(struct lexer *lex)
{
    peek_token(lex);
    if (lex->tok->type == OPERATOR && strcmp(lex->tok->data, "(") == 0)
        free_peek(lex);

    if (lex->tok->type != WORD && lex->tok->type != ASSIGNMENT_WORD)
        return NULL;
    struct ast_case_item *item = init_ast(AST_CASE_ITEM);
    vector_append(item->patterns, strdup(lex->tok->data));
    free_token(lex);

    case_item2(lex, item);
    if (lex->error != 0)
    {
        free_node(convert_node_ast(AST_CASE_ITEM, item));
        return error_handler(lex, 1, "ERROR CASE ITEM : INVALID PATTERN");
    }
    vector_append(item->patterns, NULL);

    peek_token(lex);
    if (lex->tok->type != OPERATOR || strcmp(lex->tok->data, ")") != 0)
    {
        free_node(convert_node_ast(AST_CASE_ITEM, item));
        return error_handler(lex, 1, "ERROR CASE ITEM: LACK OF \")\"");
    }
    free_token(lex);

    new_lines(lex);

    item->body = compound_list(lex);
    if (lex->error != 0)
    {
        free_node(convert_node_ast(AST_CASE_ITEM, item));
        return error_handler(lex, 1, "ERROR CASE ITEM: BAD LIST");
    }

    return convert_node_ast(AST_CASE_ITEM, item);
}

static void case_item2(struct lexer *lex, struct ast_case_item *item)
{
    peek_token(lex);
    if (lex->tok->type != OPERATOR || strcmp(lex->tok->data, "|") != 0)
        return;

    free_peek(lex);
    if (lex->tok->type != WORD && lex->tok->type != ASSIGNMENT_WORD)
    {
        error_handler(lex, 1, "ERROR CASE ITEM 2: BAD WORD");
        return;
    }

    vector_append(item->patterns, strdup(lex->tok->data));
    free_token(lex);

    case_item2(lex, item);
}

struct ast *compound_list(struct lexer *lex)
{
    new_lines(lex); // will have stock the first token non new_line in lex

    struct ast *node = and_or(lex);
    if (!node)
        return error_handler(lex, 0, "ERROR COMPOUND_LIST");
    else if (lex->error != 0)
    {
        free_node(node);
        return error_handler(lex, 1, "ERROR COMPOUND_LIST: NO MATCHIN PATTERN");
    }
    struct ast_list *list = init_ast(AST_LIST);
    add_to_list(list, node);

    compound_list2(lex, list);

    peek_token(lex);
    peek_token(lex);
    if (lex->tok->type == SEMICOLON && lex->tok2->type != SEMICOLON)
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
    peek_token(lex);
    peek_token(lex);
    if (lex->tok->type == SEMICOLON && lex->tok2->type == SEMICOLON)
        return;

    new_lines(lex);
    if (lex->tok->type == SEMICOLON)
    {
        error_handler(lex, 1,
                      "ERROR COMPOUND_LIST2: INVALID AND_OR AFTER VALID \";\"");
        return;
    }
    struct ast *node = and_or(lex);
    if (!node)
    {
        lex->error = 0;
        return;
    }
    add_to_list(list, node);

    compound_list2(lex, list);
}