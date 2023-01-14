#!/bin/sh

REF_OUT=".42suuu.txt"
TEST_OUT=".my_42suuu.txt"
blue='\033[36m'
red='\033[31m'
green='\033[32m'
nc='\033[0m'

test_lex_parse()
{
    echo "$2" > "$REF_OUT"
    ./42sh -u "$1" > "$TEST_OUT"
    var=$(diff "$REF_OUT" "$TEST_OUT")
    if [ $( echo "$var" | wc -c) -gt 1 ]; then
        echo "$red     NAN!     |  $1    ->    $2"
    else
        echo "$green      OK      |  $1    ->    $2"
    fi
}

test_input()
{
    echo "$1" | bash --posix > "$REF_OUT"
    ./42sh -c "$1" > "$TEST_OUT"
    var=$(diff "$REF_OUT" "$TEST_OUT")
    if [ $( echo "$var" | wc -c) -gt 1 ]; then
        echo "$red     NAN!     |  $1"
    else
        echo "$green      OK      |  $1"
    fi
}

test_stdin()
{
    echo "$1" | bash --posix > "$REF_OUT"
    ./42sh < "$1" > "$TEST_OUT"
    var=$(diff "$REF_OUT" "$TEST_OUT")
    if [ $( echo "$var" | wc -c) -gt 1 ]; then
        echo "$red     NAN!     |  $1"
    else
        echo "$green      OK      |  $1"
    fi
}

test_error()
{
    echo "$2" > "$REF_OUT"
    echo "$1" | ./42sh
    echo $? > "$TEST_OUT"
    var=$(diff "$REF_OUT" "$TEST_OUT")
    if [ $var -ne $2 ]; then
        echo "$red     NAN!     |  $1"
    else
        echo "$green      OK      |  $1"
    fi
}


echo $blue "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"
echo $blue "@@@@@@@@@@@@@@@@@TEST SUITE@@@@@@@@@@@@@@@@@"
echo $blue "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"
echo
echo
echo $blue "LEXER/PARSER"
test_lex_parse "test_shell/test_parser/test1.sh" "echo foo bar "
test_lex_parse "test_shell/test_parser/test2.sh" "echo foo ; echo bar "
test_lex_parse "test_shell/test_parser/test3.sh" "if true then false else true fi "
test_lex_parse "test_shell/test_parser/test4.sh" "echo foo bar ; echo wsh mon reuf bien ou bien ; if true then false else true fi "
test_lex_parse "test_shell/test_parser/test5.sh" "ls -l "
test_lex_parse "test_shell/test_parser/test6.sh" "echo "
test_lex_parse "test_shell/test_parser/test7.sh" "if true ; false ; true then echo a ; echo foooooo bar else if true ; false then echo b ; echo else echo thomas xu fi fi "
test_lex_parse "test_shell/test_parser/test8.sh" "if false then echo a else echo c fi "
test_lex_parse "test_shell/test_parser/test9.sh" "if true ; false ; true then echo a ; echo foooooo bar else if true ; false then echo b ; echo else echo thomas xu fi fi "
test_lex_parse "test_shell/test_parser/test10.sh" " "
test_lex_parse "test_shell/test_parser/test11.sh" "if false ; true ; false then echo a else echo b ; echo c echo a fi "
echo
echo $blue "STDIN"
test_stdin "test_shell/test_parser/test1.sh"
test_stdin "test_shell/test_parser/test2.sh"
test_stdin "test_shell/test_parser/test3.sh"
test_stdin "test_shell/test_parser/test4.sh"
test_stdin "test_shell/test_parser/test5.sh"
test_stdin "test_shell/test_parser/test6.sh"
test_stdin "test_shell/test_parser/test7.sh"
test_stdin "test_shell/test_parser/test8.sh"
test_stdin "test_shell/test_parser/test9.sh"
test_stdin "test_shell/test_parser/test10.sh"
test_stdin "test_shell/test_parser/test11.sh"
echo $blue "ALL INPUT"
test_input "echo foo bar"
test_input "echo foo ; echo bar"
test_input "if true; then echo ok; else echo ko; fi"
test_input "echo foo bar ; echo wsh mon reuf bien ou bien ; if true; then false; else true; fi"

test_input "ls -l"
test_input "echo "

test_input "if true ; false ; true; then echo a ; echo foooooo bar; elif true ; false; then echo b ; echo 7; else echo thomas xu; fi;"
test_input "if false; then echo a; else echo c; fi #cocuocucocuoc"
test_input "if true ; false ; true; then echo a ; echo foooooo bar; elif true ; false; then echo b ; echo 7; else echo thomas xu; fi; echo woooooo"
test_input " "
test_input "if false ; true ; false; then echo a; else echo b ; echo c; echo a; fi "
test_input 'echo \\\\\\\\'
test_input "echo toto"
test_input "echo 'coucou'"
test_input "echo -e -E foobar"
test_input "echo -n Mael arrete de geeker"
test_input "echo \"je suis une chauve souris\""
test_input "echo 2>a"
test_input "echo \2>a"
test_input "echo 2\>a"
echo
echo
echo $blue "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"
echo $blue "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"
echo $blue "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"
echo $nc
rm $REF_OUT $TEST_OUT