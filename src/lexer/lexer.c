#include "lexer.h"

#include <stdlib.h>
#include <string.h>

static int next_token_junior(struct lexer *lex, struct token *tok, char curr);
static int next_token_genZ(struct lexer *lex, struct token *tok, char *curr, char *prev);

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
    struct lexer *lex = calloc(1, sizeof(struct lexer));
    if (!lex)
        return NULL;
    lex->flags = calloc(1, sizeof(struct lex_flags));
    lex->filename = file;
    lex->tok = NULL;
    lex->error = 0;
    return lex;
}

void free_lexer(struct lexer *lex)
{
    if (lex->tok != NULL)
        free_token(lex);
    free(lex->flags);
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

static void quote(struct lexer *lex, struct token *tok, char curr)
{
    struct lex_flags *f = lex->flags;
    if (curr == '\\')
    {
        tok->data[f->i] = curr;
        curr = fgetc(lex->filename);
        if (curr == EOF)
        {
            ungetc(curr, lex->filename);
            return;
        }
        else if (curr == '\n' && f->in_dquote)
        {
            f->i--;
            return;
        }
        f->i++;
    }
    else if (f->in_squote)
    {
        if (curr == '\'')
            f->in_squote = 0;
    }
    else if (f->in_dquote)
    {
        if (curr == '\"')
            f->in_dquote = 0;
    }
    else if (curr == '\'')
        f->in_squote = 1;

    else if (curr == '\"')
        f->in_dquote = 1;

    tok->data[f->i] = curr;
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

static void rule_5(struct lexer *lex, struct token *tok, char curr)
{
    struct lex_flags *f = lex->flags;
    if (f->in_acollade)
    {
        tok->data[f->i] = curr;
        if (curr == '}')
            f->in_acollade = 1;
    }
    else
    {
        tok->data[f->i] = curr;
        curr = fgetc(lex->filename);
        if (curr == '{')
        {
            f->i++;
            tok->data[f->i] = curr;
            f->in_acollade = 1;
        }
        else
            ungetc(curr, lex->filename);
    }
}

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
    reinit_lex_flags(lex->flags, 20);
    tok->data = calloc(lex->flags->len, sizeof(char));

    // curr : current character
    char curr = tmp;
    // word flag, singlequote flag, doublequote flag, operator flag

    if (start_operator(curr))
    {
        lex->flags->was_operator = 1;
        tok->data[lex->flags->i] = curr;
        lex->flags->i++;
    }
    else
        ungetc(tmp, lex->filename);

    if (next_token_junior(lex, tok, curr))
        return;

    tok->data = realloc(tok->data, lex->flags->i + 1);
    tok->data[lex->flags->i] = '\0';
    findtype(tok, lex->flags);
    lex->tok = tok;
    //puts(lex->tok->data);
}

/*
 *return 1 if there was a comment so that the rest of next_token is not executed
 *else return 0
 */
static int next_token_junior(struct lexer *lex, struct token *tok, char curr)
{
    struct lex_flags *f = lex->flags; 
    while (1)
    {
        // previous character
        char prev = curr;
        curr = fgetc(lex->filename);

        if (curr == EOF)
        {
            if (is_invalid(lex->flags))
            {
                lex->error = 2;
                fprintf(stderr, "%s\n", "ERROR LEXER : LACK OF ENDING \", \' or }");
            }
            ungetc(curr, lex->filename);
            break;
        }
        // CHECK IF NEED TO DOUBLE SIZE
        f->len = test_data_full(&(tok->data), f->i, f->len);

        int break_lvl = next_token_genZ(lex, tok, &curr, &prev);
        if (break_lvl == 2)
            return 2;
        if (break_lvl == 1)
            break;
    }
    return 0;
}

static int next_token_genZ(struct lexer *lex, struct token *tok, char *c, char *p)
{
    struct lex_flags *f = lex->flags; 
    
    if (!f->in_squote && !f->in_dquote && f->was_operator)
    {
        if (is_operator(*p, *c))
        {
            tok->data[f->i] = *c;
            f->i++;
        }
        else
            ungetc(*c, lex->filename);
        return 1;
    }

    else if (f->in_squote || f->in_dquote || *c == '\''
            || *c == '\"' || *c == '\\')
        quote(lex, tok, *c);

    else if (f->in_acollade || *c == '$') //|| curr == '`')
        rule_5(lex, tok, *c);

    else if (!f->in_squote && !f->in_dquote && start_operator(*c))
    {
        if (is_number(tok->data))
            f->is_ionumber = 1;
        ungetc(*c, lex->filename);
        return 1;
    }

    else if (*c == ';' || *c == '\n')
    {
        ungetc(*c, lex->filename);
        return 1;
    }
    else if (my_isspace(*c))
        return 1; // RETURN TOKEN WITHOUT BLANK

    else if (f->is_word)
        tok->data[f->i] = *c;

    else if (*c == '#')
    {
        comments(lex, tok);
        return 2;
    }

    else
    {
        f->is_word = 1;
        tok->data[f->i] = *c;
    }
    f->i++;
    return 0;
}

/*
int main(int argc, char *argv[])
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