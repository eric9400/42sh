#ifndef TOKEN_H
#define TOKEN_H

enum tok_type
{
    //conditional tokens
    /*IF,
    ELSE,
    THEN,
    ELIF,
    FI,
    
    // loop
    WHILE,
    DO,
    DONE,
    UNTIL,
    FOR,
    IN,*/

    //syntax 
    SEMICOLON,
    NEWLINE,
    /*S_QUOTE,
    D_QUOTE,*/
    
    WORD,
    ASSIGNEMENT_WORD,

    END_OF_FILE,


    //all type of operators
    OP_REDIRECTION,
    OP_AND,
    OP_OR,
    OP_PIPE,
    OP_NEGATION
};

struct token
{
    enum tok_type type;
    char *data;
};

//FREE TOKEN IS IN "lexer.h" FOR PRACTICAL REASONS

#endif /*TOKEN_H*/
