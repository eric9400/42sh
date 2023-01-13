#include "lexer.h"

#include <stdlib.h>
#include <string.h>

void peek_token(struct lexer *lex)
{
    if (!lex->tok)
        next_token(lex);
}

void free_token(struct lexer *lex)
{
    free(lex->tok->data);
    free(lex->tok);
    lex->tok = NULL;
}

struct lexer *init_lexer(FILE *file)
{
    struct lexer *lex = malloc(sizeof(struct lexer));
    if (!lex)
        return NULL;
    lex->filename = file;
    lex->tok = NULL;
    lex->error = 0;
    return lex;
}

void free_lexer(struct lexer *lex)
{
    if (lex->tok != NULL)
        free_token(lex);
    free(lex);
}

static char skip_space(struct lexer *lex)
{
    // ephemere char to skip <space> type
    char tmp = fgetc(lex->filename);
    // skip <space> type
    while (my_isspace(tmp))
        tmp = fgetc(lex->filename);
    return tmp;
}

static void end_of_block_line_file(struct lexer *lex, char tmp)
{
    // printf("NEWLINE, SEMICOLON OR EOF\n");
    struct token *tok = malloc(sizeof(struct token));
    tok->data = malloc(2);
    tok->data[0] = tmp;
    tok->data[1] = '\0';
    if (tmp == '\n')
        tok->type = NEWLINE;
    else if (tmp == ';')
        tok->type = SEMICOLON;
    else
        tok->type = END_OF_FILE;
    lex->tok = tok;
}

static void quote(struct lexer *lex, struct lex_flags *flags, struct token *tok, char curr)
{
    if (curr == '\\')
    {
        tok->data[flags->i] = curr;
        curr = fgetc(lex->filename);
        if (curr == EOF)
        {
            ungetc(curr, lex->filename);
            return;
        }
        else if (curr == '\n' && flags->in_dquote)
        {
            flags->i--;
            return;
        }
        flags->i++;
    }
    else if (flags->in_squote)
    {
        if (curr == '\'')
            flags->in_squote = 0;
    }
    else if (flags->in_dquote)
    {
        if (curr == '\"')
            flags->in_dquote = 0;
    }
    else if (curr == '\'')
        flags->in_squote = 1;

    else if (curr == '\"')
        flags->in_dquote = 1;
    
    tok->data[flags->i] = curr;
}

static void comments(struct lexer *lex, struct token *tok)
{
    char curr = fgetc(lex->filename);
    while (curr != '\n' && curr != EOF)
        curr = fgetc(lex->filename);
    if (tok->data[0] == '\0')
    {
        free(tok->data);
        free(tok);
        end_of_block_line_file(lex, curr);
        return;
    }
    ungetc(curr, lex->filename);
}

static void rule_5(struct lexer *lex, struct token *tok, struct lex_flags *flags, char curr)
{
    tok->data[flags->i] = curr;
    curr = fgetc(lex->filename);
    if (curr == '{')
    {
        flags->i++;
        tok->data[flags->i] = curr;
        flags->in_variable = 1;
    }
    else
        ungetc(curr, lex->filename);
}

static int sub_next_token(struct lexer *lex, struct token *tok, char curr, struct lex_flags *flags);
// WHEN EXE IS KILL CLOSE THE FILE
void next_token(struct lexer *lex)
{
    if (lex->tok)
        return;

    // skip space types
    char tmp = skip_space(lex);
    // if first char is newline return it as a token
    if (tmp == '\n' || tmp == ';' || tmp == EOF)
    {
        end_of_block_line_file(lex, tmp);
        return;
    }

    struct token *tok = calloc(1, sizeof(struct token));
    struct lex_flags *flags = init_lex_flags(20);
    tok->data = calloc(flags->len, sizeof(char));

    // curr = current character
    char curr = tmp;
    // word flag, singlequote flag, doublequote flag, operator flag

    if (start_operator(curr))
    {
        flags->was_operator = 1;
        tok->data[flags->i] = curr;
        flags->i++;
    }
    else
        ungetc(tmp, lex->filename);
    
    if (sub_next_token(lex, tok, curr, flags))
    {
        free(flags);
        return;
    }

    tok->data = realloc(tok->data, flags->i + 1);
    tok->data[flags->i] = '\0';
    findtype(tok, flags);
    lex->tok = tok;
    free(flags);
    //puts(lex->tok->data);
}

/*
*return 1 if there was a comment so that the rest of next_token is not executed
*else return 0
*/
static int sub_next_token(struct lexer *lex, struct token *tok, char curr, struct lex_flags *flags)
{
    while (1)
    {
        // previous character
        char prev = curr;
        curr = fgetc(lex->filename);

        if (curr == EOF)
        {
            ungetc(curr, lex->filename);
            break;
        }
        // CHECK IF NEED TO DOUBLE SIZE
        flags->len = test_data_full(&(tok->data), flags->i, flags->len);

        if (!flags->in_squote && !flags->in_dquote && flags->was_operator)
        {
            if (is_operator(prev, curr))
            {
                tok->data[flags->i] = curr;
                flags->i++;
            }
            else
                ungetc(curr, lex->filename);
            break;
        }

        else if (flags->in_squote || flags->in_dquote
                 || curr == '\'' || curr == '\"' || curr == '\\')
            quote(lex, flags, tok, curr);

        else if (curr == '$') //|| curr == '`')
            rule_5(lex, tok, flags, curr);

        else if (!flags->in_squote && !flags->in_dquote && start_operator(curr))
        {
            if (is_number(tok->data))
                flags->is_ionumber = 1;
            ungetc(curr, lex->filename);
            break;
        }

        else if (curr == ';' || curr == '\n')
        {
            ungetc(curr, lex->filename);
            break;
        }
        else if (my_isspace(curr))
            break; // RETURN TOKEN WITHOUT BLANK

        else if (flags->is_word)
            tok->data[flags->i] = curr;

        else if (curr == '#')
        {
            comments(lex, tok);
            return 1;
        }

        else
        {
            flags->is_word = 1;
            tok->data[flags->i] = curr;
        }
        flags->i++;
    }
    return 0;
}

/*int main(int argc, char *argv[])
{
    if (argc != 2)
        printf("sale merde\n");
    FILE *ipf = fopen(argv[1], "r");
    if (!ipf)
        return -1;

    struct lexer *lex = init_lexer(ipf);
    next_token(lex);
    while(1)
    {
        if (lex->tok->type == END_OF_FILE)
        {
            printf("Token : EOF\t\tType : %d\n", lex->tok->type);
            break;
        }
        if (lex->tok->data[0] == '\n')
            printf("Token : \\n\t\tType : %d\n", lex->tok->type);
        else
            printf("Token : %s\t\tType : %d\n", lex->tok->data,
                   lex->tok->type);
        free(lex->tok->data);
        free(lex->tok);
        lex->tok = NULL;
        next_token(lex);
    }
    free_lexer(lex);
    fclose(ipf);
}*/