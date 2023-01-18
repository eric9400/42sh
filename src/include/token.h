#ifndef TOKEN_H
#define TOKEN_H

enum tok_type
{
    SEMICOLON,
    NEWLINE,
    IONUMBER,
    /*S_QUOTE,
    D_QUOTE,*/

    WORD,
    ASSIGNMENT_WORD,
    IO_NUMBER,

    END_OF_FILE,

    // all type of operators
    OPERATOR
};

struct token
{
    enum tok_type type;
    char *data;
};

// ALL TOKEN FUNCTIONS ARE IN "lexer.h" FOR PRACTICAL REASONS

#endif /*TOKEN_H*/
