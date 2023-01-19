#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>

#include "lexer_utils.h"
#include "token.h"

struct lexer
{
    FILE *filename;
    struct token *tok;
    struct lex_flags *flags;
    int error;
};

struct lexer *init_lexer(FILE *file);
void free_lexer(struct lexer *lex);
void next_token(struct lexer *lex);

// FOR TOKEN
void free_token(struct lexer *lex);
void peek_token(struct lexer *lex);

#endif /*LEXER_H*/