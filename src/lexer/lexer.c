#include "token.h"
#include "lexer.h"

static FILE *f = NULL;
static enum QUOTE_STATE in_quote = 0;

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

//WHEN EXE IS KILL CLOSE THE FILE
struct Token *next_token(void)
{
    struct Token *res = malloc(sizeof(Token));

    int len = 20;
    int i = 0;
    res->data = malloc(sizeof(char) * len);

    int was_operator = 0;
    char prev = '\0';
    char curr = '\0';
    int is_word = 0;
    while (1)
    {
        char prev = curr;
        curr = fgetc(f);
        len = test_data_full(res->data, len); //CHECK IF NEED TO DOUBLE SIZE

        if (c == '\0' || c == EOF)
            break;

        else if (in_quote == FIRST)
        {
            if (curr == ''')
            {
                lseek(f, -1, SEEK_CUR);
                in_quote = LAST;
                //RETURN TOKEN FROM DATA
            }
            res->data[i] = curr;
            i++;
        }

        else if (in_quote == LAST)
        {
            in_quote = NONE;
            //RETURN ' TOKEN
        }
        /*
        else if (!in_quote && was_operator)
        {
            // char curr = is_operator(prev, curr);
            puts("TODO");
        }
        */
        else if (curr == ''')
        {
            in_quote = FIRST;
            //RETURN ' TOKEN
        }

        else if (curr == ';' || curr == '\n')
        {
            if (i != 0)
            {
                fseek(f, -1, SEEK_CUR);
                //RETURN TOKEN FROM RES DATA
            }
            else
            {
                curr == '\n' ? return /* \n TOKEN */ : return /* ; TOKEN*/;
                //RETURN ; TOKEN
            }
        }

        // rule 5

        /*else if (!in_quote && start_op(curr))
        {
            // RETURN TOKEN
        }*/

        else if (isspace(curr))
        {
            if (i == 0)
                continue;
            // RETURN TOKEN WITHOUT BLANK
        }

        else if (is_word)
            res->data[i] = curr;

        else if (curr == '#')
        {
            curr = fgetc(f);
            while (curr != '\n' && curr != '\0' && curr != EOF)
                curr = fgetc(f);
        }

        else
        {
            is_word = 1;
            res->data[i] = curr;
        }
        i++;
    }
    res->data = realloc(res->data, i + 1);
    res->data[i] = '\0';
    find_type(res);
    return res;
}
