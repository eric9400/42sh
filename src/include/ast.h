#ifndef AST_H
#define AST_H

struct ast_cmd
{
    
};

struct ast_list
{

};

struct ast_if
{
    struct ast_list *condition;
    struct ast_cmd *then;
    struct ast_cmd *else_body;
};

union ast_union
{
    struct ast_cmd ast_cmd;
    struct ast_list ast_list;
    struct ast_if ast_if;
};

struct ast
{
    enum ast-type type;
    union ast_union data;
};

#endif /* AST_H */
