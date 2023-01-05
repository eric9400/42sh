#ifndef TOKEN_H
#define TOKEN_H

enum Token_type
{
    //conditional tokens
    IF, ELSE, THEN, ELIF, FI,

    //syntax 
    SEMICOLON, NEWLINE, S_QUOTE, 
    
    WORDS
};

struct Token
{
    enum Token_type t_type;
    char *t_value;
};

#endif /*TOKEN_H*/