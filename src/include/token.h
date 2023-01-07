#ifndef TOKEN_H
#define TOKEN_H


enum tok_type
{
    //conditional tokens
    IF, ELSE, THEN, ELIF, FI,

    //syntax 
    SEMICOLON, NEWLINE, S_QUOTE,

    WORD,

    END_OF_FILE
};

struct token
{
    enum tok_type type;
    char *data;
};

#endif /*TOKEN_H*/
