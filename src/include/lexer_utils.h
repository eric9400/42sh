#ifndef LEXER_UTILS_H
#define LEXER_UTILS_H

#include <stdio.h>
#include <stdlib.h>

#include "token.h"

struct lex_flags
{
    // index dans length of token->data
    int i;
    int len;

    // flags
    int is_word;
    int in_squote;
    int in_dquote;
    int was_operator;
    int in_variable;
    int in_acollade;
    int is_ionumber;
    int found_backslash;
};

struct lex_flags *init_lex_flags(int len);
void findtype(struct token *tok, struct lex_flags *flags);
int test_data_full(char **data, int i, int len);
int is_operator(char p, char c);
int start_operator(char c);
int is_number(char *str);
int my_isspace(char c);

#endif /*LEXER_UTILS_H*/