#ifndef AST_INIT_H
#define AST_INIT_H

#include "ast.h"
#include "vector.h"

struct ast_cmd *init_cmd(void);
struct ast_list *init_list(size_t capacity);
struct ast_if *init_if(void);
struct ast_while *init_while(void);
struct ast_until *init_until(void);
struct ast_prefix *init_prefix(void);
struct ast_redir *init_redir(void);
struct ast_element *init_element(void);
struct ast_sp_cmd *init_sp_cmd(void);
struct ast_sh_cmd *init_sh_cmd(void);

#endif /* AST_INIT_H */