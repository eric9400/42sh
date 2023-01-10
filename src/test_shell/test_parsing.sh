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
    ./42sh -cp "$1" > "$TEST_OUT"
    var=$(diff "$REF_OUT" "$TEST_OUT")
    if [ $( echo "$var" | wc -c) -gt 1 ]; then
        echo "$red    |  $1    ->    $2:      NAN!"
    else
        echo "$green    |  $1    ->    $2:      OK"
    fi
}

test_error()
{
    echo "$2" > "$REF_OUT"
    echo "$1" | ./42sh
    echo $? > "$TEST_OUT"
    var=$(diff "$REF_OUT" "$TEST_OUT")
    if [ $var -ne $2 ]; then
        echo "$red    |  $1  :      NAN!"
    else
        echo "$green    |  $1  :      OK"
    fi
}

echo $blue "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"
echo $blue "@@@@@@@@@@@@@@@@@TEST SUITE@@@@@@@@@@@@@@@@@"
echo $blue "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"
echo
echo
test_lex_parse "test_shell/test1.sh" "echo foo bar "
test_lex_parse "test_shell/test2.sh" "echo foo ; echo bar "

test_lex_parse "test_shell/test3.sh" "if true then false else true fi "
test_lex_parse "test_shell/test4.sh" "echo foo bar ; echo wsh mon reuf bien ou bien ; if true then false else true fi "

test_lex_parse "test_shell/test5.sh" "ls -l "
test_lex_parse "test_shell/test6.sh" "echo "

test_lex_parse "test_shell/test7.sh" "if true ; false ; true then echo a; echo foooooo bar else if true ; false then echo b; echo else echo thomas xu fi "
test_lex_parse "test_shell/test8.sh" "if false then echo a else echo c fi "
test_lex_parse "test_shell/test9.sh" "if true ; false ; true then echo a ; echo foooooo bar else if true ; false then echo b; echo else echo thomas xu fi "
test_lex_parse "test_shell/test10.sh" "if false true then echo a ; echo b ; echo c fi"
test_lex_parse "test_shell/test11.sh" "if false; true; false then echo a else echo b echo c echo a fi "

echo
echo
echo $blue "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"
echo $blue "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"
echo $blue "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"
echo $nc
rm $REF_OUT $TEST_OUT
