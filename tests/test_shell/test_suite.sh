#!/bin/sh

REF_OUT=".42suuu.txt"
TEST_OUT=".my_42suuu.txt"
blue='\033[36m'
red='\033[31m'
green='\033[32m'
nc='\033[0m'
p_all=0

test_lex_parse()
{
    echo "$2" > "$REF_OUT"
    ./42sh -u "$1" > "$TEST_OUT"
    var=$(diff "$REF_OUT" "$TEST_OUT")
    if [ $(echo "$var" | wc -c) -gt 1 ]; then
        echo "$red     NAN!     |  $1    ->    $2"
    elif [ $p_all -eq 1 ]; then
        echo "$green      OK      |  $1    ->    $2"
    fi
}

test_input()
{
    echo "$1" | bash --posix > "$REF_OUT"
    ./42sh -c "$1" > "$TEST_OUT"
    var=$(diff "$REF_OUT" "$TEST_OUT")
    if [ $(echo "$var" | wc -c) -gt 1 ]; then
        echo "$red     NAN!     |  $1"
    elif [ $p_all -eq 1 ]; then
        echo "$green      OK      |  $1"
    fi
}

test_stdin()
{
    echo "$1" | bash --posix > "$REF_OUT"
    ./42sh < "$1" > "$TEST_OUT"
    var=$(diff "$REF_OUT" "$TEST_OUT")
    if [ $(echo "$var" | wc -c) -gt 1 ]; then
        echo "$red     NAN!     |  $1"
    elif [ $p_all -eq 1 ]; then
        echo "$green      OK      |  $1"
    fi
}


test_error()
{
    echo "$1" | bash --posix > /dev/null 2>&1
    echo $? > "$REF_OUT"
    ./42sh -c "$1" > /dev/null 2>&1
    echo $? > "$TEST_OUT"
    var=$(diff "$REF_OUT" "$TEST_OUT")
    if [ $(echo "$var" | wc -c) -gt 1 ]; then
        echo "$red     NAN!     |  $1"
    elif [ $p_all -eq 1 ]; then
        echo "$green      OK      |  $1"
    fi
}

echo $blue "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"
echo $blue "@@@@@@@@@@@@@@@@@TEST SUITE@@@@@@@@@@@@@@@@@"
echo $blue "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"
echo
echo
echo $blue "===================STEP 1==================="
#echo 
#echo $blue "LEXER/PARSER"
# test_lex_parse "test_shell/step1/test1.sh" "echo foo bar "
# test_lex_parse "test_shell/step1/test2.sh" "echo foo ; echo bar "
# test_lex_parse "test_shell/step1/test3.sh" "if true then false else true fi "
# test_lex_parse "test_shell/step1/test4.sh" "echo foo bar ; echo wsh mon reuf bien ou bien ; if true then false else true fi "
# test_lex_parse "test_shell/step1/test5.sh" "ls -l "
# test_lex_parse "test_shell/step1/test6.sh" "echo "
# test_lex_parse "test_shell/step1/test7.sh" "if true ; false ; true then echo a ; echo foooooo bar else if true ; false then echo b ; echo else echo thomas xu fi fi "
# test_lex_parse "test_shell/step1/test8.sh" "if false then echo a else echo c fi "
# test_lex_parse "test_shell/step1/test9.sh" "if true ; false ; true then echo a ; echo foooooo bar else if true ; false then echo b ; echo else echo thomas xu fi fi "
# test_lex_parse "test_shell/step1/test10.sh" " "
# test_lex_parse "test_shell/step1/test11.sh" "if false ; true ; false then echo a else echo b ; echo c echo a fi "
echo
echo $blue "STDIN" $red
test_stdin "test_shell/step1/test1.sh"
test_stdin "test_shell/step1/test2.sh"
test_stdin "test_shell/step1/test3.sh"
test_stdin "test_shell/step1/test4.sh"
test_stdin "test_shell/step1/test5.sh"
test_stdin "test_shell/step1/test6.sh"
test_stdin "test_shell/step1/test7.sh"
test_stdin "test_shell/step1/test8.sh"
test_stdin "test_shell/step1/test9.sh"
test_stdin "test_shell/step1/test10.sh"
test_stdin "test_shell/step1/test11.sh"
test_stdin "test_shell/step1/if_else_comment.sh"
test_stdin "test_shell/step1/if_else_comment2.sh"
test_stdin "test_shell/step1/if_else.sh"
test_stdin "test_shell/step1/echo_backslash_dquote.sh"
test_stdin "test_shell/step1/backlash_newline.sh"
test_stdin "test_shell/step1/backlash_newline2.sh"
test_stdin "test_shell/step1/ascii_house.sh"
#test_stdin "test_shell/step1/cursed.sh"
echo
echo $blue "ALL INPUT" $red
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
test_input "if true; then echo foo; fi"
test_input 'echo \\\\\\\\'
test_input "echo toto"
test_input "echo 'coucou'"
test_input "echo -e -E foobar"
test_input "echo -n Mael arrete de geeker"
test_input 'echo \"je suis une chauve souris\"'
test_input 'echo \"\\n\"'
test_input "echo -e '\n'"
test_input 'echo -E \n"'
test_input 'echo -E \"\n\"'
test_input "echo -E '\n'"
test_input "'coucou'"
echo
echo $blue "AMOUNG SUS ERRORS" $red
test_error "if"
test_error "if true; echo toto; fi"
test_error "coucou"
test_error "coucou;"
test_error "echo world; coucou; echo foo"
test_error "echo world; echo foo; coucou"
test_error "if gg; then okkqqqqyy;"
test_error "if thtrue then; fi"
test_error "if true; then echo fi"
test_error "if ; fi"
test_error "if; if ; if ; if ; then; fi"
test_error "if ; else; coucou"
test_error "if ; then echo ok ; fi"
test_error "if true; then; echo ko; fi"
test_error 'if ;\n true; then; flase; fi;fi;fi;fi'
test_error 'if ;\n\n\n\ntrue;\n then echo okqyyy; fi'
test_error "then; fi;"
#test_error "echo if; if echo"
test_error "if true; then false; elif true; then true; else false;"
test_error "if true;;;;;;;;;then false; fi"
test_error "echo 'aaa"
test_error 'echo "aaa'
echo
echo $blue "===================STEP 2==================="
echo 
echo $bleu "INPUT" $red
test_input "if true || false; then echo foo; fi"
test_input "if true || false && true && false; then echo foo; else echo bar; fi"
test_input "while false; do echo foofoooo; done"
test_input "echo foobar | echo"
test_input "! echo foobar | echo | echo | echo | echo"
test_input "if true && true; then while false; do echo bar; done; fi"
test_input "echo toto 0>a"
test_input "echo 2>a"
test_input 'echo \2>a'
test_input 'echo 2\>a'
test_input 'until true; do echo clement; done'
echo
echo
echo $blue "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"
echo $blue "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"
echo $blue "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"
echo $nc
rm $REF_OUT $TEST_OUT a
