#!/bin/sh

REF_OUT=".42suuu.txt"
TEST_OUT=".my_42suuu.txt"
blue='\033[35m'
red='\033[31m'
green='\033[32m'
nc='\033[0m'
pass=0
test=0
fail=0
p_all=0

test_input()
{
    test=$(($test+1));
    bash --posix -c "$1" > "$REF_OUT" 2> /dev/null
    ref=$(echo $?);
    ./42sh -c "$1" > "$TEST_OUT" 2> /dev/null
    tst=$(echo $?);
    var=$(diff "$REF_OUT" "$TEST_OUT")
    if [ $(echo "$var" | wc -c) -gt 1 ] || [ $tst -ne $ref ]; then
        fail=$(($fail+1));
        echo "$red     NAN!     |\tR: $ref \tT: $tst\t|  $1"
    elif [ $p_all -eq 1 ]; then
        echo "$green      OK      |  $1"
    else
        pass=$(($pass+1));
    fi
}


test_stdin()
{
    test=$(($test+1));
    bash --posix "$1" > "$REF_OUT" 2> /dev/null
    ref=$(echo $?);
    ./42sh "$1" > "$TEST_OUT" 2> /dev/null
    tst=$(echo $?);
    var=$(diff "$REF_OUT" "$TEST_OUT")
    if [ $(echo "$var" | wc -c) -gt 1 ] || [ $tst -ne $ref ]; then
        fail=$(($fail+1));
        echo "$red     NAN!     |\tR: $ref \tT: $tst\t|  $1"
    elif [ $p_all -eq 1 ]; then
        echo "$green      OK      |  $1"
    else
        pass=$(($pass+1));
    fi
}

test_stdin2()
{
    test=$(($test+1));
    bash --posix < "$1" > "$REF_OUT" 2> /dev/null
    ref=$(echo $?);
    ./42sh < "$1" > "$TEST_OUT" 2> /dev/null
    tst=$(echo $?);
    var=$(diff "$REF_OUT" "$TEST_OUT")
    if [ $(echo "$var" | wc -c) -gt 1 ] || [ $tst -ne $ref ]; then
        fail=$(($fail+1));
        echo "$red     NAN!     |\tR: $ref \tT: $tst\t|  $1"
    elif [ $p_all -eq 1 ]; then
        echo "$green      OK      |  $1"
    else
        pass=$(($pass+1));
    fi
}

test_error()
{
    test=$(($test+1));
    bash --posix -c "$1" > /dev/null 2> /dev/null
    ref=$(echo $?);
    if [ $ref -eq 1 ]; then
        ref=$(($ref+1));
    fi;
    ./42sh -c "$1" > /dev/null 2> /dev/null
    tst=$(echo $?);
    if [ $tst -ne $ref ]; then
        fail=$(($fail+1));
        echo "$red     NAN!     |\tR: $ref \tT: $tst\t|  $1"
    elif [ $p_all -eq 1 ]; then
        echo "$green      OK      |  $1"
    else
        pass=$(($pass+1));
    fi
}

test_error2()
{
    test=$(($test+1));
    bash --posix -c "$1" > /dev/null 2> /dev/null
    ref=$(echo $?);
    ./42sh -c "$1" > /dev/null 2> /dev/null
    tst=$(echo $?);
    if [ $tst -ne $ref ]; then
        fail=$(($fail+1));
        echo "$red     NAN!     |\tR: $ref \tT: $tst\t|  $1"
    elif [ $p_all -eq 1 ]; then
        echo "$green      OK      |  $1"
    else
        pass=$(($pass+1));
    fi
}

test_stdin_error()
{
    test=$(($test+1));
    bash --posix < "$1" > /dev/null 2>&1
    ref=$(echo $?);
    ./42sh < "$1" > /dev/null 2>&1
    tst=$(echo $?)
    if [ $tst -ne $ref ]; then
        fail=$(($fail+1));
        echo "$red     NAN!     |\tR: $ref \tT: $tst\t|  $1"
    elif [ $p_all -eq 1 ]; then
        echo "$green      OK      |  $1"
    else
        pass=$(($pass+1));
    fi
}

test_stdin_error2()
{
    test=$(($test+1));
    bash --posix < "$1" > /dev/null 2>&1
    ref=$(echo $?);
    if [ $ref -eq 1 ]; then
        ref=$(($ref+1));
    fi;
    ./42sh < "$1" > /dev/null 2>&1
    tst=$(echo $?)
    if [ $tst -ne $ref ]; then
        fail=$(($fail+1));
        echo "$red     NAN!     |\tR: $ref \tT: $tst\t|  $1"
    elif [ $p_all -eq 1 ]; then
        echo "$green      OK      |  $1"
    else
        pass=$(($pass+1));
    fi
}

echo $blue "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"
echo $blue "@@@@@@@@@@@@@@@@@TEST SUITE@@@@@@@@@@@@@@@@@"
echo $blue "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"
echo
echo
echo $blue "===================STEP 1==================="
echo
echo $blue " SCRIPTS" $red
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
test_stdin2 "test_shell/step1/test1.sh"
test_stdin2 "test_shell/step1/test2.sh"
test_stdin2 "test_shell/step1/test3.sh"
test_stdin2 "test_shell/step1/test4.sh"
test_stdin2 "test_shell/step1/test5.sh"
test_stdin2 "test_shell/step1/test6.sh"
test_stdin2 "test_shell/step1/test7.sh"
test_stdin2 "test_shell/step1/test8.sh"
test_stdin2 "test_shell/step1/test9.sh"
test_stdin2 "test_shell/step1/test10.sh"
test_stdin2 "test_shell/step1/test11.sh"
test_stdin2 "test_shell/step1/if_else_comment.sh"
test_stdin2 "test_shell/step1/if_else_comment2.sh"
test_stdin2 "test_shell/step1/if_else.sh"
test_stdin2 "test_shell/step1/echo_backslash_dquote.sh"
test_stdin2 "test_shell/step1/backlash_newline.sh"
test_stdin2 "test_shell/step1/backlash_newline2.sh"
test_stdin2 "test_shell/step1/ascii_house.sh"
test_stdin_error "test_shell/step1/command_list_err.sh"
test_stdin_error "test_shell/block_suite/test_1_1.sh"
test_stdin_error "test_shell/block_suite/test_1_2.sh"
test_stdin_error "test_shell/block_suite/test_1_3.sh"
test_stdin_error "test_shell/block_suite/test_1_4.sh"
test_stdin_error "test_shell/block_suite/test_1_5.sh"
test_stdin_error "test_shell/block_suite/test_1_6.sh"
test_stdin_error "test_shell/block_suite/test_1_7.sh"
test_stdin_error "test_shell/block_suite/test_1_8.sh"
test_stdin_error "test_shell/block_suite/test_1_9.sh"
test_stdin_error "test_shell/block_suite/test_1_10.sh"
#test_stdin "test_shell/step1/cursed.sh"
echo
echo $blue " ALL INPUT" $red
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
test_input "'echo toto'"
test_input "echo 'toto'"

test_input "echo '\n'"
test_input "echo \"\n\""
test_input "echo $1"
test_input "echo \"\n\""
test_input "echo 'a   \'b\z' \'c\z"
test_input "if coucou; then echo toto; else echo tata; fi"

test_input "'#'quote"
test_input "'ls'"
echo
echo $blue " AMOUNG SUS ERRORS" $red
test_error 'if'
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
test_error "if true; then echo foo;; fi"
test_error "if true;; then echo foo; echo bar; fi"
test_error "echo foo; echo bar;;"
test_error "if false; then echo toto; else echo foo; echo bar;; fi"
test_error "then; fi;"
test_error "if"
test_error "if; then; fi"
test_error "if true; then fi; fi"
test_error 'if \n; then echo foo; fi'
test_error 'if true\n; then echo foo; fi'
test_error "if true; then false;"
test_error "if true; then false;;"
test_error "if true; then; fi"
test_error "if true true true; then;; false; fi"
test_error "if false; then echo toto; elif true; fi"
test_error "if false; then echo toto; elif true; then; fi"
test_error "if false; then echo toto; elif true; then echo tata fi"
test_error "if false; then echo toto; elif true; then echo tata; else echo fi"
test_error "if false; then echo toto; elif true; then echo tata; else echo foo;"
#test_error "echo if; if echo"
test_error "if true; then false; elif true; then true; else false;"
test_error "if true;;;;;;;;;then false; fi"
test_error "echo 'aaa"
test_error 'echo "aaa'
test_error "''"
test_error "' '"
test_error " "
test_error "' arg '"
test_error "'if' true; then echo toto; fi"
test_error2 "false"
test_error2 "\"false\""
test_error2 "\'false\'"
test_error2 "'false'"
test_error2 "false echo foo"
echo
echo $blue "===================STEP 2==================="
echo 
echo $blue " SCRIPTS"
test_stdin "test_shell/step2/for1_10.sh"
test_stdin "test_shell/step2/for12345.sh"
test_stdin "test_shell/step2/pipe.sh"
test_stdin "test_shell/step2/var.sh"
test_stdin "test_shell/step2/while_if.sh"
test_stdin2 "test_shell/step2/for1_10.sh"
test_stdin2 "test_shell/step2/for12345.sh"
test_stdin2 "test_shell/step2/pipe.sh"
test_stdin2 "test_shell/step2/var.sh"
test_stdin2 "test_shell/step2/while_if.sh"
test_stdin_error "test_shell/block_suite/test_2_1.sh"
test_stdin_error "test_shell/block_suite/test_2_2.sh"
test_stdin_error "test_shell/block_suite/test_2_3.sh"
test_stdin_error "test_shell/block_suite/test_2_4.sh"
#test_stdin_error "test_shell/block_suite/test_2_5.sh"
test_stdin_error "test_shell/block_suite/test_2_6.sh"
test_stdin_error "test_shell/block_suite/test_2_7.sh"
test_stdin_error "test_shell/block_suite/test_2_8.sh"
test_stdin_error "test_shell/block_suite/test_2_9.sh"
test_stdin_error "test_shell/block_suite/test_2_10.sh"
test_stdin_error "test_shell/block_suite/test_2_11.sh"
test_stdin_error "test_shell/block_suite/test_2_12.sh"
test_stdin_error "test_shell/block_suite/test_2_13.sh"
test_stdin_error "test_shell/block_suite/test_2_14.sh"
test_stdin_error "test_shell/block_suite/test_2_15.sh"
test_stdin_error "test_shell/block_suite/test_2_16.sh"
test_stdin_error "test_shell/block_suite/test_2_17.sh"
test_stdin_error "test_shell/block_suite/test_2_18.sh"
test_stdin_error "test_shell/block_suite/test_2_19.sh"
test_stdin_error "test_shell/block_suite/test_2_20.sh"
test_stdin_error "test_shell/block_suite/test_2_21.sh"
test_stdin_error "test_shell/block_suite/test_2_22.sh"
test_stdin_error "test_shell/block_suite/test_2_23.sh"
test_stdin_error "test_shell/block_suite/test_2_24.sh"
test_stdin_error "test_shell/block_suite/test_2_25.sh"
test_stdin_error "test_shell/block_suite/test_2_26.sh"
test_stdin_error "test_shell/block_suite/test_2_27.sh"
#test_stdin_error "test_shell/block_suite/test_2_28.sh"
test_stdin_error "test_shell/block_suite/test_2_29.sh"
test_stdin_error "test_shell/block_suite/test_2_30.sh"
test_stdin_error "test_shell/block_suite/test_2_31.sh"
test_stdin_error "test_shell/block_suite/test_2_32.sh"
test_stdin_error "test_shell/block_suite/test_2_33.sh"
echo
echo $blue " INPUT" $red
test_input "echo \"roger, \" \"bois ton ricard !\"; echo \"dis donc roger tes bourre ou quoi\""
test_input "echo \"\""
test_input "echo \"toto\""
test_input "echo \'toto\'"
test_input "echo \"   \"  \\\"'\""
test_input "echo \"aaaaa\nbbbbbb\""
test_input "echo \" $ \\\\a \\\\b \\\\z \\\\ \""
test_input "echo $'\"\"\"\"\"\"\"\"'"
test_input "echo \"\\\\ abcdefghijklmnopqrstuvwxyz0123456789/*-+ &*()_-+=|!\\\\@#$%^&\""
test_input "echo \"\"\"''\\\\\\\\\"'\"'\"''''\"\\\\\"\\\\'\\\\\\\\$'\\\\$\"'\""
test_input "echo \"\\\\$'te*st'\""
test_input "echo \"'c'\\\\''est l' \\\\' 'le bonheur' \\\\ ''\""
test_input "echo \"\\\\#escaped\""
test_input "echo \"cont\\\\\nent\""
test_input "echo \"quoi?\nfeur\""
test_input "for n; do echo foo; done"
test_input "for coucou; do echo foo; done"
test_input "n=5; for \$n; do echo foo; done"
test_input "n=5;\n for \$n; do echo foo; done"
test_input "for n in foo bar; do echo toto; done"
test_input "n=4; foo=7; for \$n in \$foo; do echo toto; done"
test_input "! true"
test_input "! echo foo"
test_input "! echo foo | echo"
test_input "! echo foobar | echo | echo | echo | echo"
test_input "true || false"
test_input "true && false"
test_input "true && false || true || false && echo bar"
test_input "true && true && true && true"
test_input "true &&\n false"
test_input "if true || false; then echo foo; fi"
test_input "if true || false && true && false; then echo foo; else echo bar; fi"
test_input "echo ffoo | echo"
test_input "echo foobar | echo"
test_input "echo foo | echo | echo | echo | echo"
test_input "ls | echo | echo | echo"
test_input "echo toto 0>a"
rm a
test_input "echo 2>b"
rm b
test_input 'echo \2>c'
rm c
test_input 'echo 2\>d'
#rm d
test_input "echo foo >e"
rm e
test_input "echo foo >&f"
rm f
test_input "echo foo >>g"
rm g
test_input "echo foo <>h"
rm h
test_input "echo foo <i"
#rm i
test_input "echo foo >|j"
rm j
test_input "2>file echo 1>file2 foo 0>file3 bar"
rm file file2 file3
test_input "while false; do echo foofoooo; done"
test_input "while false; do echo foo; echo fooooo; echo fooooo; done"
test_input "while 0; do ls; done"
test_input "if true && true; then while false; do echo bar; done; fi"
test_input 'until true; do echo clement; done'
test_input "until true; do echo foofoooo; done"
test_input "until true; do echo foo; echo fooooo; echo fooooo; done"
test_input "until true; do ls; done"

test_input "a=1"
test_input "b=c"
test_input "\$b=a"
test_input "echo \$c"
test_input "a=toto"
test_input "export \$a=AAAAAAAAAAAAAAAAAA"
test_input "env"
test_input "exit"
echo
echo $blue " SUS ERRORS" $red
test_error "\"roger, \" \"bois ton ricard !\"; echo \"dis donc roger tes bourre ou quoi\""
test_error "\"\""
test_error "\"toto\""
test_error "\'toto\'"
test_error "\"   \"  \\\"'\""
test_error "\"aaaaa\nbbbbbb\""
test_error "\" $ \\\\a \\\\b \\\\z \\\\ \""
test_error "$'\"\"\"\"\"\"\"\"'"
test_error "\"\\\\ abcdefghijklmnopqrstuvwxyz0123456789/*-+ &*()_-+=|!\\\\@#$%^&\""
test_error "\"\"\"''\\\\\\\\\"'\"'\"''''\"\\\\\"\\\\'\\\\\\\\$'\\\\$\"'\""
test_error "\"\\\\$'te*st'\""
test_error "\"'c'\\\\''est l' \\\\' 'le bonheur' \\\\ ''\""
test_error "\"\\\\\#escaped\""
test_error "\"cont\\\\\nent\""
test_error "\"quoi?\nfeur\""

test_error "for n do echo foo; done"
test_error "for coucou;;; do echo foo; done"
test_error "for $n; do echo foo; done"
test_error "for $n; echo foo; done"
test_error "for n; do echo foo;"
test_error "for n in foo bar do echo toto; done"
test_error "for $n in $foo; do echo toto; done"

test_error "fooo |"
test_error "| true"
test_error "true | | false"
test_error "! echo foo |"
test_error "true ||"
test_error "false && "
test_error "|| false"
test_error "&& true"
test_error "|| ||"
test_error "|| false ||"
test_error "|| true ||"
test_error "|| || true"
test_error "false || ||"
test_error "|| &&"
test_error "&& false &&"
test_error "|| true &&"
test_error "&& && true"
test_error "false && &&"
test_error "false || |"
test_error "false || true |"
test_error "|| |"
test_error "|||"
test_error "! "
test_error2 "! false"
test_error2 "! true"
test_error2 "! echo foo"
test_error2 "! coucou"
test_error "true \n&& false"

test_error "echo foo |"
test_error "echo foo >"
test_error "echo foo >>"
test_error "echo foo <"
test_error "echo foo 2>"

test_error "while ; do echo foo; done"
test_error "while false; echo foo; done"
test_error "while false; do echo foo;"
test_error "while false; do echo foo done"
test_error "until ; do echo foo; done"
test_error "until true; echo foo; done"
test_error "until true; do echo foo;"
test_error "until true; do echo foo done"

test_error "$a"
test_error "a=b; $a"
test_error "a=====================f; $a"
test_error "$$$$$$$$$$$$$$==D"
echo
echo $blue "RONALDO SCORED [$green $pass $blue] TIMES"
echo $blue "RONALDO FAILED [$red $fail $blue] TIMES"
echo $blue "OUT OF [$nc $test $blue] TESTS!!"
echo
echo
echo $blue "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"
echo $blue "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"
echo $blue "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"
echo $nc
rm $REF_OUT $TEST_OUT