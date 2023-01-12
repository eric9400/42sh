#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>

#include "token.h"

struct lexer
{
    FILE *filename;
    struct token *tok;
    int error;
};

struct lex_flags
{
    int is_word;
    int in_squote;
    int in_dquote;
    int was_operator;
    int in_variable;
    int is_ionumber;
};

struct lex_flags *init_lex_flags(void);

struct lexer *init_lexer(FILE *file);
void free_lexer(struct lexer *lex);
void next_token(struct lexer *lex);

// FOR TOKEN
void free_token(struct lexer *lex);
void peek_token(struct lexer *lex);

#endif /*LEXER_H*/
