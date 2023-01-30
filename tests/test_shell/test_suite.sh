#!/bin/sh

color=0 #1 if there is not a color default in echo  
p_all=0 #print all test even the ones that passes

REF_OUT=".42suuu.txt"
TEST_OUT=".my_42suuu.txt"
blue='\033[1;35m'
red='\033[1;31m'
green='\033[1;32m'
nc='\033[1;0m'
pass=0
test=0
fail=0

if [ $color -eq 1 ]; then
    alias ecco='echo -e'
else
    alias ecco='echo'
fi

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
        ecco "$red     NAN!     |\tR: $ref \tT: $tst\t|  $1"
    elif [ $p_all -eq 1 ]; then
        ecco "$green      OK      |  $1"
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
        ecco "$red     NAN!     |\tR: $ref \tT: $tst\t|  $1"
    elif [ $p_all -eq 1 ]; then
        ecco "$green      OK      |  $1"
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
        ecco "$red     NAN!     |\tR: $ref \tT: $tst\t|  $1"
    elif [ $p_all -eq 1 ]; then
        ecco "$green      OK      |  $1"
    else
        pass=$(($pass+1));
    fi
}

test_var_arg()
{
    test=$(($test+1));
    bash --posix "$1" $2 $3 > "$REF_OUT" 2> /dev/null
    ref=$(echo $?);
    ./42sh "$1" $2 $3 > "$TEST_OUT" 2> /dev/null
    tst=$(echo $?);
    var=$(diff "$REF_OUT" "$TEST_OUT")
    if [ $(echo "$var" | wc -c) -gt 1 ] || [ $tst -ne $ref ]; then
        fail=$(($fail+1));
        ecco "$red     NAN!     |\tR: $ref \tT: $tst\t|  $1"
    elif [ $p_all -eq 1 ]; then
        ecco "$green      OK      |  $1"
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
        ecco "$red     NAN!     |\tR: $ref \tT: $tst\t|  $1"
    elif [ $p_all -eq 1 ]; then
        ecco "$green      OK      |  $1"
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
        ecco "$red     NAN!     |\tR: $ref \tT: $tst\t|  $1"
    elif [ $p_all -eq 1 ]; then
        ecco "$green      OK      |  $1"
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
        ecco "$red     NAN!     |\tR: $ref \tT: $tst\t|  $1"
    elif [ $p_all -eq 1 ]; then
        ecco "$green      OK      |  $1"
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
        ecco "$red     NAN!     |\tR: $ref \tT: $tst\t|  $1"
    elif [ $p_all -eq 1 ]; then
        ecco "$green      OK      |  $1"
    else
        pass=$(($pass+1));
    fi
}

ecco $blue "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"
ecco $blue "@@@@@@@@@@@@@@@@@TEST SUITE@@@@@@@@@@@@@@@@@"
ecco $blue "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"
ecco
ecco
ecco $blue "===================STEP 1==================="
ecco
ecco $blue " SCRIPTS" $red

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
test_stdin "test_shell/step1/cursed.sh"
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
test_stdin2 "test_shell/step1/cursed.sh"

ecco
ecco $blue " ALL INPUT" $red

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

ecco
ecco $blue " AMOUNG SUS ERRORS" $red

test_stdin_error "test_shell/step1/command_list_err.sh"
test_stdin_error "test_shell/bad_suite/test_1_1.sh"
test_stdin_error "test_shell/bad_suite/test_1_2.sh"
test_stdin_error "test_shell/bad_suite/test_1_3.sh"
test_stdin_error "test_shell/bad_suite/test_1_4.sh"
test_stdin_error "test_shell/bad_suite/test_1_5.sh"
test_stdin_error "test_shell/bad_suite/test_1_6.sh"
test_stdin_error "test_shell/bad_suite/test_1_7.sh"
test_stdin_error "test_shell/bad_suite/test_1_8.sh"
test_stdin_error "test_shell/bad_suite/test_1_9.sh"
test_stdin_error "test_shell/bad_suite/test_1_10.sh"

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
test_error 'echo -E \n"'
#
#
#
#
#
#
#
#
#
#
#
#
#
#
#
#
#
#
#
ecco
ecco $blue "===================STEP 2==================="
ecco 
ecco $blue " SCRIPTS"

test_stdin "test_shell/step2/for1_10.sh"
test_stdin "test_shell/step2/for12345.sh"
test_stdin "test_shell/step2/pipe.sh"
test_stdin "test_shell/step2/var.sh"
test_stdin "test_shell/step2/while_if.sh"
test_stdin "test_shell/step2/test1.sh"
test_stdin "test_shell/step2/test2.sh"
test_stdin "test_shell/step2/test3.sh"
test_stdin "test_shell/step2/test4.sh"
test_stdin "test_shell/step2/test6.sh"
test_stdin "test_shell/step2/test7.sh"
test_stdin "test_shell/step2/test8.sh"
test_stdin "test_shell/step2/test9.sh"
test_stdin "test_shell/step2/test10.sh"
test_stdin "test_shell/step2/test11.sh"
test_stdin "test_shell/step2/test12.sh"
test_stdin "test_shell/step2/test14.sh"
test_stdin "test_shell/step2/test17.sh"
test_stdin "test_shell/step2/test18.sh"
test_stdin "test_shell/step2/test19.sh"
test_stdin "test_shell/step2/test20.sh"
test_stdin "test_shell/step2/test21.sh"
test_stdin "test_shell/step2/test22.sh"
test_stdin "test_shell/step2/test23.sh"
test_stdin "test_shell/step2/test24.sh"
test_stdin "test_shell/step2/test25.sh"
test_stdin "test_shell/step2/test26.sh"
test_stdin "test_shell/step2/test27.sh"
test_stdin "test_shell/step2/test28.sh"
test_stdin "test_shell/step2/test29.sh"
test_stdin2 "test_shell/step2/for1_10.sh"
test_stdin2 "test_shell/step2/for12345.sh"
test_stdin2 "test_shell/step2/pipe.sh"
test_stdin2 "test_shell/step2/var.sh"
test_stdin2 "test_shell/step2/while_if.sh"
test_stdin2 "test_shell/step2/test1.sh"
test_stdin2 "test_shell/step2/test2.sh"
test_stdin2 "test_shell/step2/test3.sh"
test_stdin2 "test_shell/step2/test4.sh"
test_stdin2 "test_shell/step2/test6.sh"
test_stdin2 "test_shell/step2/test7.sh"
test_stdin2 "test_shell/step2/test8.sh"
test_stdin2 "test_shell/step2/test9.sh"
test_stdin2 "test_shell/step2/test10.sh"
test_stdin2 "test_shell/step2/test11.sh"
test_stdin2 "test_shell/step2/test12.sh"
test_stdin2 "test_shell/step2/test14.sh"
test_stdin2 "test_shell/step2/test17.sh"
test_stdin2 "test_shell/step2/test18.sh"
test_stdin2 "test_shell/step2/test19.sh"
test_stdin2 "test_shell/step2/test20.sh"
test_stdin2 "test_shell/step2/test21.sh"
test_stdin2 "test_shell/step2/test22.sh"
test_stdin2 "test_shell/step2/test23.sh"
test_stdin2 "test_shell/step2/test24.sh"
test_stdin2 "test_shell/step2/test26.sh"
test_stdin2 "test_shell/step2/test27.sh"
test_stdin2 "test_shell/step2/test28.sh"
test_stdin2 "test_shell/step2/test29.sh"
test_var_arg "test_shell/step2/var_arg1.sh" 'a b' c
test_var_arg "test_shell/step2/var_arg2.sh" 'a b' c
test_var_arg "test_shell/step2/var_arg3.sh" 'a b' c
test_var_arg "test_shell/step2/var_arg4.sh" 'a b' c

ecco
ecco $blue " INPUT" $red

test_input "echo \"roger, \" \"bois ton ricard !\"; echo \"dis donc roger tes bourre ou quoi\""
test_input "echo \"\""
test_input "echo \"toto\""
test_input "echo \'toto\'"
test_input "echo \"aaaaa\nbbbbbb\""
test_input "echo \" $ \\\\a \\\\b \\\\z \\\\ \""
test_input "echo \"\\\\ abcdefghijklmnopqrstuvwxyz0123456789/*-+ &*()_-+=|!\\\\@#$%^&\""
test_input "echo \"\\\\$'te*st'\""
test_input "echo \"'c'\\\\''est l' \\\\' 'le bonheur' \\\\ ''\""
test_input "echo \"\\\\#escaped\""
test_input "echo \"cont\\\\\nent\""
test_input "echo \"quoi?\nfeur\""
test_input "for n; do echo foo; done"
test_input "for coucou; do echo foo; done"
test_input "for n in foo bar; do echo toto; done"
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
test_input "c=3; echo \$c"
test_input "echo \$@"
test_input "echo \$*"
test_input "echo \$?"
test_input "echo \$1"
test_input "echo \$#"
test_input "echo \$UID"
test_input "echo \$OLDPWD"
test_input "echo \$IFS"
test_input "echo \$@\$@"
test_input "echo \"\$@\""
test_input "echo \"\$@\""
test_input "echo \$c"
test_input "echo \$c"
test_input "echo \$c"
test_input "\'echo \$#\'"
test_input "a=toto"

ecco
ecco $blue " SUS ERRORS" $red

test_stdin_error "test_shell/bad_suite/test_2_1.sh"
test_stdin_error "test_shell/bad_suite/test_2_2.sh"
test_stdin_error "test_shell/bad_suite/test_2_3.sh"
test_stdin_error "test_shell/bad_suite/test_2_4.sh"
test_stdin_error "test_shell/bad_suite/test_2_6.sh"
test_stdin_error "test_shell/bad_suite/test_2_7.sh"
test_stdin_error "test_shell/bad_suite/test_2_8.sh"
test_stdin_error "test_shell/bad_suite/test_2_9.sh"
test_stdin_error "test_shell/bad_suite/test_2_10.sh"
test_stdin_error "test_shell/bad_suite/test_2_11.sh"
test_stdin_error "test_shell/bad_suite/test_2_12.sh"
test_stdin_error "test_shell/bad_suite/test_2_13.sh"
test_stdin_error "test_shell/bad_suite/test_2_14.sh"
test_stdin_error "test_shell/bad_suite/test_2_15.sh"
test_stdin_error "test_shell/bad_suite/test_2_16.sh"
test_stdin_error "test_shell/bad_suite/test_2_17.sh"
test_stdin_error "test_shell/bad_suite/test_2_18.sh"
test_stdin_error "test_shell/bad_suite/test_2_19.sh"
test_stdin_error "test_shell/bad_suite/test_2_20.sh"
test_stdin_error "test_shell/bad_suite/test_2_21.sh"
test_stdin_error "test_shell/bad_suite/test_2_22.sh"
test_stdin_error "test_shell/bad_suite/test_2_23.sh"
test_stdin_error "test_shell/bad_suite/test_2_24.sh"
test_stdin_error "test_shell/bad_suite/test_2_25.sh"
test_stdin_error "test_shell/bad_suite/test_2_26.sh"
test_stdin_error "test_shell/bad_suite/test_2_27.sh"
test_stdin_error "test_shell/bad_suite/test_2_29.sh"
test_stdin_error "test_shell/bad_suite/test_2_30.sh"
test_stdin_error "test_shell/bad_suite/test_2_31.sh"
test_stdin_error "test_shell/bad_suite/test_2_32.sh"
test_stdin_error "test_shell/bad_suite/test_2_33.sh"

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
test_error "echo \"\"\"''\\\\\\\\\"'\"'\"''''\"\\\\\"\\\\'\\\\\\\\$'\\\\$\"'\""
test_error "\"\\\\$'te*st'\""
test_error "\"'c'\\\\''est l' \\\\' 'le bonheur' \\\\ ''\""
test_error "\"\\\\\#escaped\""
test_error "\"cont\\\\\nent\""
test_error "\"quoi?\nfeur\""
test_error "echo \"   \"  \\\"'\""

test_error "for n do echo foo; done"
test_error "for coucou;;; do echo foo; done"
test_error "for $n; do echo foo; done"
test_error "for $n; echo foo; done"
test_error "for n; do echo foo;"
test_error "for n in foo bar do echo toto; done"
test_error "for $n in $foo; do echo toto; done"
test_error "n=5; for \$n; do echo foo; done"
test_error "n=5;\n for \$n; do echo foo; done"
test_error "n=4; foo=7; for \$n in \$foo; do echo toto; done"

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
test_error2 "true \n&& false"

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
#
#
#
#
#
#
#
#
#
#
#
#
#
#
#
#
#
#
#
ecco
ecco $blue "===================STEP 3==================="
ecco 
ecco $blue " SCRIPTS"

test_stdin "test_shell/step3/block_list.sh"
test_stdin "test_shell/step3/continue.sh"
test_stdin "test_shell/step3/not_multilpe_pipes.sh"
test_stdin "test_shell/step3/var_multiple_words.sh"
test_stdin "test_shell/step3/var_substitute_embraces1.sh"
test_stdin "test_shell/step3/weird_single_double_quotes2.sh"
test_stdin "test_shell/step3/weird_single_double_quotes1.sh"
test_stdin "test_shell/step3/function.sh"
test_stdin "test_shell/step3/test1.sh"
test_stdin "test_shell/step3/test2.sh"
test_stdin "test_shell/step3/test3.sh"
test_stdin "test_shell/step3/test4.sh"
test_stdin "test_shell/step3/test5.sh"
test_stdin "test_shell/step3/test6.sh"
test_stdin "test_shell/step3/test7.sh"
test_stdin "test_shell/step3/test8.sh"
test_stdin "test_shell/step3/test9.sh"
test_stdin "test_shell/step3/test10.sh"
test_stdin "test_shell/step3/test11.sh"
test_stdin "test_shell/step3/test12.sh"
test_stdin "test_shell/step3/test14.sh"
test_stdin "test_shell/step3/test17.sh"
test_stdin "test_shell/step3/test18.sh"
test_stdin "test_shell/step3/test19.sh"
test_stdin "test_shell/step3/test20.sh"
test_stdin "test_shell/step3/test21.sh"
test_stdin "test_shell/step3/test22.sh"
test_stdin "test_shell/step3/test23.sh"
test_stdin "test_shell/step3/test24.sh"
test_stdin "test_shell/step3/test25.sh"
test_stdin "test_shell/step3/test26.sh"
test_stdin "test_shell/step3/test27.sh"
test_stdin "test_shell/step3/test28.sh"
test_stdin "test_shell/step3/test29.sh"
test_stdin "test_shell/step3/test30.sh"
test_stdin "test_shell/step3/test31.sh"
test_stdin "test_shell/step3/test32.sh"
test_stdin "test_shell/step3/test33.sh"
test_stdin "test_shell/step3/test34.sh"
test_stdin "test_shell/step3/test35.sh"
test_stdin "test_shell/step3/test36.sh"
test_stdin "test_shell/step3/test37.sh"
test_stdin "test_shell/step3/test38.sh"
test_stdin "test_shell/step3/test40.sh"
test_stdin "test_shell/step3/test44.sh"
test_stdin "test_shell/step3/test45.sh"
test_stdin "test_shell/step3/test46.sh"
test_stdin "test_shell/step3/test47.sh"
test_stdin "test_shell/step3/test48.sh"
test_stdin "test_shell/step3/test49.sh"
test_stdin "test_shell/step3/test51.sh"
test_stdin "test_shell/step3/test52.sh"
test_stdin "test_shell/step3/test53.sh"
test_stdin "test_shell/step3/test54.sh"
test_stdin "test_shell/step3/test55.sh"
test_stdin "test_shell/step3/test56.sh"
test_stdin "test_shell/step3/test57.sh"
test_stdin "test_shell/step3/test58.sh"
test_stdin "test_shell/step3/test59.sh"
test_stdin "test_shell/step3/test61.sh"
test_stdin "test_shell/step3/test62.sh"
test_stdin "test_shell/step3/test63.sh"
test_stdin "test_shell/step3/test64.sh"
test_stdin "test_shell/step3/test65.sh"
test_stdin "test_shell/step3/test66.sh"
test_stdin "test_shell/step3/test67.sh"
test_stdin "test_shell/step3/test68.sh"
test_stdin "test_shell/step3/test70.sh"
test_stdin "test_shell/step3/test72.sh"
test_stdin "test_shell/step3/case.sh"
test_stdin "test_shell/step3/case2.sh"
test_stdin "test_shell/step3/case3.sh"
test_stdin "test_shell/step3/case4.sh"
test_stdin "test_shell/step3/case_error1.sh"
test_stdin "test_shell/step3/case_error2.sh"
test_stdin "test_shell/step3/case_error3.sh"
test_stdin "test_shell/step3/case_error4.sh"
test_stdin "test_shell/step4/alias1.sh"
test_stdin "test_shell/step4/alias2.sh"
test_stdin "test_shell/step4/alias3.sh"
test_stdin "test_shell/step4/alias4.sh"

test_stdin2 "test_shell/step3/block_list.sh"
test_stdin2 "test_shell/step3/continue.sh"
test_stdin2 "test_shell/step3/not_multilpe_pipes.sh"
test_stdin2 "test_shell/step3/var_multiple_words.sh"
test_stdin2 "test_shell/step3/var_substitute_embraces1.sh"
test_stdin2 "test_shell/step3/weird_single_double_quotes2.sh"
test_stdin2 "test_shell/step3/weird_single_double_quotes1.sh"
test_stdin2 "test_shell/step3/function.sh"
test_stdin2 "test_shell/step3/test1.sh"
test_stdin2 "test_shell/step3/test2.sh"
test_stdin2 "test_shell/step3/test3.sh"
test_stdin2 "test_shell/step3/test4.sh"
test_stdin2 "test_shell/step3/test5.sh"
test_stdin2 "test_shell/step3/test6.sh"
test_stdin2 "test_shell/step3/test7.sh"
test_stdin2 "test_shell/step3/test8.sh"
test_stdin2 "test_shell/step3/test9.sh"
test_stdin2 "test_shell/step3/test10.sh"
test_stdin2 "test_shell/step3/test11.sh"
test_stdin2 "test_shell/step3/test12.sh"
test_stdin2 "test_shell/step3/test14.sh"
test_stdin2 "test_shell/step3/test17.sh"
test_stdin2 "test_shell/step3/test18.sh"
test_stdin2 "test_shell/step3/test19.sh"
test_stdin2 "test_shell/step3/test20.sh"
test_stdin2 "test_shell/step3/test21.sh"
test_stdin2 "test_shell/step3/test22.sh"
test_stdin2 "test_shell/step3/test23.sh"
test_stdin2 "test_shell/step3/test24.sh"
test_stdin2 "test_shell/step3/test25.sh"
test_stdin2 "test_shell/step3/test26.sh"
test_stdin2 "test_shell/step3/test27.sh"
test_stdin2 "test_shell/step3/test28.sh"
test_stdin2 "test_shell/step3/test29.sh"
test_stdin2 "test_shell/step3/test30.sh"
test_stdin2 "test_shell/step3/test31.sh"
test_stdin2 "test_shell/step3/test32.sh"
test_stdin2 "test_shell/step3/test33.sh"
test_stdin2 "test_shell/step3/test34.sh"
test_stdin2 "test_shell/step3/test35.sh"
test_stdin2 "test_shell/step3/test36.sh"
test_stdin2 "test_shell/step3/test37.sh"
test_stdin2 "test_shell/step3/test38.sh"
test_stdin2 "test_shell/step3/test40.sh"
test_stdin2 "test_shell/step3/test44.sh"
test_stdin2 "test_shell/step3/test45.sh"
test_stdin2 "test_shell/step3/test46.sh"
test_stdin2 "test_shell/step3/test47.sh"
test_stdin2 "test_shell/step3/test48.sh"
test_stdin2 "test_shell/step3/test49.sh"
test_stdin2 "test_shell/step3/test51.sh"
test_stdin2 "test_shell/step3/test52.sh"
test_stdin2 "test_shell/step3/test53.sh"
test_stdin2 "test_shell/step3/test54.sh"
test_stdin2 "test_shell/step3/test55.sh"
test_stdin2 "test_shell/step3/test56.sh"
test_stdin2 "test_shell/step3/test57.sh"
test_stdin2 "test_shell/step3/test58.sh"
test_stdin2 "test_shell/step3/test59.sh"
test_stdin2 "test_shell/step3/test61.sh"
test_stdin2 "test_shell/step3/test62.sh"
test_stdin2 "test_shell/step3/test63.sh"
test_stdin2 "test_shell/step3/test64.sh"
test_stdin2 "test_shell/step3/test65.sh"
test_stdin2 "test_shell/step3/test66.sh"
test_stdin2 "test_shell/step3/test67.sh"
test_stdin2 "test_shell/step3/test68.sh"
test_stdin2 "test_shell/step3/test70.sh"
test_stdin2 "test_shell/step3/test72.sh"
test_stdin2 "test_shell/step3/case.sh"
test_stdin2 "test_shell/step3/case2.sh"
test_stdin2 "test_shell/step3/case3.sh"
test_stdin2 "test_shell/step3/case4.sh"
test_stdin2 "test_shell/step3/case_error1.sh"
test_stdin2 "test_shell/step3/case_error2.sh"
test_stdin2 "test_shell/step3/case_error3.sh"
test_stdin2 "test_shell/step3/case_error4.sh"

test_stdin2 "test_shell/step4/alias1.sh"
test_stdin2 "test_shell/step4/alias2.sh"
test_stdin2 "test_shell/step4/alias3.sh"
test_stdin2 "test_shell/step4/alias4.sh"

ecco
ecco $blue " INPUTE"

test_input "{ echo foo; echo bar; echo coucou; }"
test_input "{ echo toto;}"
test_input "{ echo foo; }"
test_input "{ \necho foo; \necho bar;\n echo tutu\n; }"
test_input "{ if true; then echo toto; elif false; then echo bar; fi; echo chop; }"
test_input "{ echo laviedmamere; while false; do echo foo; echo fooooo; echo fooooo; done; echo coucou; }"
test_input "{ { echo suuuuu; } }"
test_input "echo foo; { echo bar; { echo suuuuu; } }"

test_input "exit"
test_input "echo foo; exit"
test_input "exit; echo foo; echo tooooooo;"
test_input "while true; do echo toto; exit; done"
test_input "exit; exit; exit; exit"

origin=$(pwd)
test_input "cd test_shell/; pwd; cd .."
test_input "cd test_shell/; echo foooo; pwd; cd .."
test_input "cd; pwd; cd $origin"
test_input "cd .; pwd"
test_input "cd test_shell/../test_shell/../; pwd; cd $origin"
test_input "cd test_shell/../test_shell/../../..; pwd; cd $origin"

test_input "export aaa=AAAAAAAAAAAAAAAAA; env | grep \"aaa=\""
test_input "export aaa=AAAAAAAAAAAAAAAAAA; env | grep \"abbb=\""
test_input "aaa=EINSTEINLAFRAUDE; export aaa; env | grep \"aaa=\""

test_input "export aaa=AAA; unset aaa; env | grep \"aaa=\""
test_input "export bba=AAAA cca=BBBB; unset bba cca; env | grep \"aaa=\""
test_input "export dda=AAA; unset -v dda; env | grep \"a=\""
test_input "babaji () { echo toto; } ; unset -f babaji; babaji"

test_input ". ./test_shell/test_lexer/src1.sh"
test_input ". ./test_shell/test_lexer/src2.sh"
test_input ". ./test_shell/test_lexer/src_empty.sh"
test_input ". ./test_shell/test_lexer/src_err.sh"

test_input "while true; do echo toto; break; done"
test_input "for n in 1 2 3; do echo \$n; if true; then break; fi ; done"
test_input "for n in 1 2 3; do echo \$n; if true; then continue; fi ; done"

test_input "bababa () { echo tutu; }"
test_input "bababa () { echo tutu; }; bababa"
test_input "babaji () { echo \$1; }; babaji coucou"
test_input "babaji; { babaji () { echo coucou; } ; }"
test_input "babaji () { echo \$1; babaji2 () { echo wsh \$1; babaji3 () { echo homme tron \$1; } ; } ; }; babaji eric; babaji2 mael; babaji3 paul;"
test_input "if true; then ntm () { echo toktoktoktkt; } ; fi; ntm"
test_input "babaji () { babaji2 () { babaji3 () { echo foo;} ; } ; }; babaji; babaji2; babaji3"

test_input "\$(echo toto); echo tata"
test_input "\$(if true; then a=5; echo $a; fi)"
test_input "echo foo; \$(if true || false && true && false; then echo foo; else echo bar; fi)"
test_input "\$(echo foo; echo bar; \$(echo toto;)); ls"
test_input "\$(\$(\$(\$(\$(\$(\$(echo foo;))))))); echo bar"
test_input "\$(echo \"je suis pas une parenthese:)\" non plus:\) )"
test_input "while true; do echo toto; \$(echo dada); break; done"
test_input 'a=5; echo $a; $(a=1000); echo $a'
test_input 'a=5; echo $a; $(a=1000; echo toto); echo $a'

test_input "(echo toto)"
test_input "(echo toto; (echo tata))"
test_input "echo toto; (echo tata; (echo tutu; (echo 4)))"
test_input "if ( echo foo; echo bar) && true; then echo tata; fi"
test_input "(echo toto)
echo foo"

ecco
ecco $blue " SUS ERRORS"

test_stdin_error "test_shell/bad_suite/test_3_1.sh"
test_stdin_error "test_shell/bad_suite/test_3_2.sh"
test_stdin_error "test_shell/bad_suite/test_3_3.sh"
test_stdin_error "test_shell/bad_suite/test_3_4.sh"

test_error "{ echo coucou; echo bar;"
test_error "}"
test_error "}}"
test_error "{"
test_error "{{"
test_error "{{{{{{{{{"
test_error "echo coucou; echo foo; }"
test_error "if {echo foo; echo bar; echo tuu; } && true; then echo foooooo; fi"
test_error "{ echo foo; echo bar; echo coucou }"
test_error "{echo toto;}"
test_error "{ echo toto}"

test_error2 "export \$a=AAAAAAAAAAAAAAAAAA"

test_error "bababa () { echo tutu }"
test_error "bababa ( { echo tutu }"
test_error "bababa ) { echo tutu }"
test_error "bababa { echo tutu }"
test_error "bababa () echo foo"

test_error "\$(echo toto"
test_error "\$)"
test_error "\$()"
test_error "\$(((((("

test_error "("
test_error ")"
test_error "((((echo toto;)))"
test_error "((echo tata))))"
test_error "(echo toto)"
test_error "echo (echo (echo (echo 4)))"
test_error "(echo toto)\n echo foo"
test_input "{echo foo; { echo bar; { echo suuuuu; } }"


test_stdin "test_shell/step2/test13.sh"
test_stdin "test_shell/step2/test15.sh"
test_stdin "test_shell/step2/test16.sh"
test_stdin2 "test_shell/step2/test13.sh"
test_stdin2 "test_shell/step2/test15.sh"
test_stdin2 "test_shell/step2/test16.sh"
test_stdin "test_shell/step3/test_export_1.sh"
test_stdin "test_shell/step3/test_export_2.sh"
test_stdin "test_shell/step3/test16.sh"
test_stdin "test_shell/step3/test39.sh"
test_stdin "test_shell/step3/test41.sh"
test_stdin "test_shell/step3/test42.sh"
test_stdin "test_shell/step3/test43.sh"
test_stdin2 "test_shell/step3/test_export_1.sh"
test_stdin2 "test_shell/step3/test_export_2.sh"
test_stdin2 "test_shell/step3/test13.sh"
test_stdin2 "test_shell/step3/test39.sh"
test_stdin2 "test_shell/step3/test41.sh"
test_stdin2 "test_shell/step3/test42.sh"
test_stdin2 "test_shell/step3/test43.sh"
#test_input "export dda=AAAA bba=BBBB cca=CCCCCCC; env | grep \"a=\""
#test_input "export aaa=AAA; aaa=BBB; unset -n aaa; env | grep \"aaa=\""
test_stdin "test_shell/step3/test13.sh"
test_stdin "test_shell/step3/test15.sh"
test_stdin2 "test_shell/step3/test15.sh"
test_stdin2 "test_shell/step3/test16.sh"
#
#ecco
#ecco $blue " HARDCORE"

# test_stdin_error "test_shell/bad_suite/test_2_5.sh"
# test_stdin_error "test_shell/bad_suite/test_2_28.sh"
# test_stdin2 "test_shell/step2/test25.sh"
# test_error "echo if; if echo"
# test_stdin "test_shell/step3/test69.sh"
# test_stdin "test_shell/step3/test71.sh"
# test_stdin2 "test_shell/step3/test69.sh"
# test_stdin2 "test_shell/step3/test71.sh"
# test_input "echo $'\"\"\"\"\"\"\"\"'"
# test_input "echo \$\$"
# test_stdin "test_shell/step2/test5.sh"
# test_stdin2 "test_shell/step2/test5.sh"
#
ecco
ecco $blue "RONALDO SCORED [$green $pass $blue] TIMES"
ecco $blue "RONALDO FAILED [$red $fail $blue] TIMES"
ecco $blue "OUT OF [$nc $test $blue] TESTS!!"
ecco
ecco
ecco $blue "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"
ecco $blue "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"
ecco $blue "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"
ecco $nc
unalias ecco
rm $REF_OUT $TEST_OUT
