#ifndef TOKEN_H
#define TOKEN_H

enum tok_type
{
    SEMICOLON = 0,
    NEWLINE = 1,
    IONUMBER = 2,

    WORD = 3,
    ASSIGNMENT_WORD = 4,
    IO_NUMBER = 5,

    END_OF_FILE = 6,

    // all type of operators
    OPERATOR = 7
};

struct token
{
    enum tok_type type;
    char *data;
};

// ALL TOKEN FUNCTIONS ARE IN "lexer.h" FOR PRACTICAL REASONS

#endif /*TOKEN_H*/
