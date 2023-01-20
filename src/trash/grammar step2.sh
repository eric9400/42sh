STEP 2

input =
    list '\n'
    | list EOF
    | '\n'
    | EOF
    ;

list = and_or { ';' and_or } [ ';' ] ;

and_or = pipeline { '&&' | '||' {'\n'} pipeline } ;

pipeline = ['!'] command { '|' {'\n'} command } ;



command =
    simple_command
    | shell_command { redirection }
    | funcdec { redirection }
    ;

redirection = [IONUMBER] '>' | '<' | '>>' | '>&' | '<&' | '>|' | '<>' WORD ;

funcdec = WORD '(' ')' {'\n'} shell_command ;



simple_command =
    prefix { prefix }
    | { prefix } WORD { element }
    ;

prefix =
    ASSIGNMENT_WORD
    | redirection
    ;

element =
    WORD
    | redirection
    ;