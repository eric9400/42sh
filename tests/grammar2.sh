shell_command =
    '{' compound_list '}'
    | '(' compound_list ')'
    | rule_if
    | rule_while
    | rule_until
    | rule_for
    ;

rule_if = 'if' compound_list 'then' compound_list [else_clause] 'fi' ;

rule_while = 'while' compound_list 'do' compound_list 'done' ;

rule_until = 'until' compound_list 'do' compound_list 'done' ;

rule_for =
  'for' WORD [';'] | [ {'\n'} 'in' { WORD } ';' | '\n' ] {'\n'} 'do' compound_list 'done' ;

else_clause =
    'else' compound_list
    | 'elif' compound_list 'then' compound_list [else_clause]
    ;

compound_list = {'\n'} and_or { ';' | '\n' {'\n'} and_or } [';'] {'\n'} ;

compound_list_spe = {'\n'} and_or ';' | '\n' {{'\n'} and_or ';' | '\n'} {'\n'} ;