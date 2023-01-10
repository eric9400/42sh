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
        echo "$red    |  $1  :      NAN!"
    else
        echo "$green    |  $1  :      OK"
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
test_lex_parse "test_shell/test1.sh" "echo foo bar (null) "
test_lex_parse "test_shell/test2.sh" "echo foo (null) ; echo bar (null) "
test_lex_parse "test_shell/test3.sh" "if true then false else true fi "
test_lex_parse "test_shell/test4.sh" "echo foo bar (null) ; echo wsh mon reuf bien ou bien (null) ; if true then false else true fi "
test_lex_parse "test_shell/test5.sh" "ls -l (null) "
test_lex_parse "test_shell/test6.sh" "echo (null) "
echo
echo
echo $blue "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"
echo $blue "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"
echo $blue "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"
echo $nc
rm $REF_OUT $TEST_OUT
