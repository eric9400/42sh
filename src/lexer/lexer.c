#include "token.h"

static FILE *f = NULL;

static void findtype(struct Token *token)
{
    if (!strcmp(token->data,"if"))
        token->t_type = IF;
    else if (!strcmp(token->data,"else"))
        token->t_type = ELSE;
    else if (!strcmp(token->data,"then"))
        token->t_type = THEN;
    else if (!strcmp(token->data,"fi"))
        token->t_type = FI;
    else if (!strcmp(token->data,"elif"))
        token->t_type = ELIF;

    else if (!strcmp(token->data,"'"))
        token->t_type = S_QUOTE;
    else if (!strcmp(token->data,";"))
        token->t_type = SEMICOLON;
    else if (!strcmp(token->data,"\n"))
        token->t_type = NEWLINE;
    
    else
        token->t_type = WORDS;
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

void init_file(FILE *file)
{
    f = file;
}

struct Token *next_token(void)
{
    struct Token *res = malloc(sizeof(Token));

    int len = 20;
    int i = 0;
    res->data = malloc(sizeof(char) * len);

    int in_quote = 0;
    int was_operator = 0;
    char prev = '\0';
    char curr = '\0';
    while (1)
    {
        char prev = curr;
        curr = fgetc(f);

        if (c == '\0' || c == EOF)
            break;

        else if (!in_quote && was_operator)
        {
            // char curr = is_operator(prev, curr);
            puts("TODO");
        }

        else if ()
        i++;
    }
    res->data = realloc(res->data, i + 1);
    res->data[i] = '\0';
    find_type(res);
}
