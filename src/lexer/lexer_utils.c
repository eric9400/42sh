#include "lexer_utils.h"

struct lex_flags *init_lex_flags(int len)
{
    struct lex_flags *flags = calloc(1, sizeof(struct lex_flags));
    flags->i = 0;
    flags->len = len;
    flags->is_word = 0;
    flags->in_squote = 0;
    flags->in_dquote = 0;
    flags->was_operator = 0;
    flags->in_variable = 0;
    flags->in_acollade = 0;
    flags->is_ionumber = 0;
    flags->found_backslash = 0;
    return flags;
}

static int is_name(char *str, size_t size)
{
    if (str[0] != '_' && (str[0] < 'a' || str[0] > 'z')
        && (str[0] < 'A' || str[0] > 'Z'))
        return 0;
    for (size_t i = 1; i < size; i++)
    {
        if (str[0] != '_' && (str[0] < 'a' || str[0] > 'z')
            && (str[0] < 'A' || str[0] > 'Z')
            && (str[0] < '0' || str[0] > '9'))
            return 0;
    }
    return 1;
}

static int is_assign_word(struct token *tok)
{
    char *str = tok->data;
    for (size_t i = 0; str[i] != '\0'; i++)
    {
        if (str[i] == '=')
            return is_name(str, i);
    }
    return 0;
}

void findtype(struct token *tok, struct lex_flags *flags)
{
    if (!tok->data)
        return;
    else if (flags->is_ionumber)
        tok->type = IO_NUMBER;
    else if (flags->was_operator)
        tok->type = OPERATOR;
    else if (is_assign_word(tok))
        tok->type = ASSIGNMENT_WORD;
    else if (flags->is_word)
        tok->type = WORD;
    else
        tok->type = WORD;
}

int test_data_full(char **data, int i, int len)
{
    if (i >= len)
    {
        len *= 2;
        *data = realloc(*data, len);
    }
    return len;
}

/*
 * if prev and curr can be associated : return new char
 * else return curr
 */
int is_operator(char p, char c)
{
    return (p == '|' && c == '|') || (p == '&' && c == '&')
        || (p == '>' && c == '>') || (p == '>' && c == '&')
        || (p == '<' && c == '&') || (p == '>' && c == '|')
        || (p == '<' && c == '>');
}

int start_operator(char c)
{
    return c == '!' || c == '|' || c == '&' || c == '>' || c == '<';
}

int is_number(char *str)
{
    int is_num = 1;
    for (size_t i = 0; str[i] != '\0' && is_num; i++)
        is_num = (str[i] >= '0' && str[i] <= '9');
    return is_num;
}

int my_isspace(char c)
{
    return c == ' ' || c == '\t';
}
