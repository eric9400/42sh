#include "lexer.h"

#include <stdlib.h>
#include <string.h>

#include "token.h"

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

static int test_data_full(char **data, int i, int len)
{
    if (i >= len)
    {
        len *= 2;
        *data = realloc(*data, len);
    }
    return len;
}

static void findtype(struct token *tok, int is_word)
{
    if (!tok->data)
        return;
    else if (is_word)
        tok->type = WORD;
    else
        tok->type = WORD;
}

/*
 * if prev and curr can be associated : return new char
 * else return curr
 */
/*
char is_operator(char prev, char curr)
{
    // TODO
}
*/

static int my_isspace(char c)
{
    return c == ' ' || c == '\t';
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
    //printf("NEWLINE, SEMICOLON OR EOF\n");
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

//WHEN EXE IS KILL CLOSE THE FILE
void next_token(struct lexer *lex)
{
    if(lex->tok)
        return;

    //skip space types
    char tmp = skip_space(lex);
    //if first char is newline return it as a token
    if (tmp == '\n' || tmp == ';' || tmp == EOF)
    {
        end_of_block_line_file(lex, tmp);
        return;
    }
    ungetc(tmp, lex->filename);
    //fseek(lex->filename, -1, SEEK_CUR);

    struct token *tok = malloc(sizeof(struct token));
    int len = 20;
    int i = 0;
    tok->data = malloc(sizeof(char) * len);

    // curr = current character et prev = previous character
    //char prev = '\0';
    char curr = '\0';
    // word flag, singlequote flag, doublequote flag
    int is_word = 0;
    int in_squote = 0;
    int in_dquote = 0;

    while (1)
    {
        curr = fgetc(lex->filename);

        if (curr == EOF)
        {
            ungetc(curr, lex->filename);
            break;
        }
        //CHECK IF NEED TO DOUBLE SIZE
        len = test_data_full(&(tok->data), i, len);

        /*
        else if (!in_squote && !in_dquote && was_operator)
        {
            // char curr = is_operator(prev, curr);
            puts("TODO");
        }
        */

        if (in_squote)
        {
            if (curr == '\'')
                in_squote = 0;
            tok->data[i] = curr;
        }

        else if (in_dquote)
        {
            if (curr == '\"')
                in_dquote = 0;
            tok->data[i] = curr;
        }

        else if (curr == '\'')
        {
            in_squote = 1;
            tok->data[i] = curr;
        }

        else if (curr == '\"')
        {
            in_dquote = 1;
            tok->data[i] = curr;
        }

        else if (curr == '\\')
        {
            tok->data[i] = curr;
            curr = fgetc(lex->filename);
            tok->data[i] = curr;
            break;
        }

        else if (curr == ';' || curr == '\n')
        {
            //fseek(lex->filename, -1, SEEK_CUR);
            ungetc(curr, lex->filename);
            break;
        }

        // rule 5
        //TODO

        /*else if (!in_quote && start_op(curr))
        {
            //TODO
            // RETURN TOKEN
        }*/

        else if (my_isspace(curr))
            break; // RETURN TOKEN WITHOUT BLANK

        else if (is_word)
            tok->data[i] = curr;

        else if (curr == '#')
        {
            curr = fgetc(lex->filename);
            while (curr != '\n' && curr != '\0' && curr != EOF)
                curr = fgetc(lex->filename);
            free(tok->data);
            free(tok);
            end_of_block_line_file(lex, curr);
            return;
        }

        else
        {
            is_word = 1;
            tok->data[i] = curr;
        }
        i++;
    }
    tok->data = realloc(tok->data, i + 1);
    tok->data[i] = '\0';
    findtype(tok, is_word);
    lex->tok = tok;
    //puts(lex->tok->data);
}

/*
int main(void)
{
    FILE *ipf = fopen("test.sh", "r");
    if (!ipf)
        return -1;

    struct lexer *lex = init_lexer(ipf);
    next_token(lex);
    while(1)
    {
        if (lex->tok->type == END_OF_FILE)
        {
            printf("Token : \"EOF\"\t\tType : %d\n", lex->tok->type);
            break;
        }
        if (lex->tok->data[0] == '\n')
            printf("Token : \"\\n\"\t\tType : %d\n", lex->tok->type);
        else
            printf("Token : \"%s\"\t\tType : %d\n", lex->tok->data, lex->tok->type);
        free(lex->tok->data);
        free(lex->tok);
        lex->tok = NULL;
        next_token(lex);
    }
    free_lexer(lex);
    fclose(ipf);
}*/
