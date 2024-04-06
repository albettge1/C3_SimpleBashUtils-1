#!/bin/bash

SUCCESS=0
FAIL=0
COUNTER=0
DIFF_RES=""

declare -a tests=(
"test tesst.txt test2.txt"
"-e test tesst.txt"
"-i TEST tesst.txt"
"-v test tesst.txt"
"-c test tesst.txt"
"-l test tesst.txt test2.txt test3.txt"
"-n test tesst.txt"

"-o fest tesst.txt"
"-h test tesst.txt test2.txt"
"-s test tesst.txt"
"-f patterns_grep.txt test2.txt"

"-cv test tesst.txt"
"-iv test tesst.txt"
"-lv test tesst.txt test2.txt test3.txt"
"-ho test test2.txt"
)

testing()
{
    t=$(echo $@ | sed "s/VAR/$var/")
    ./s21_grep $t > test_s21_grep.log
    grep $t > test_sys_grep.log
    DIFF_RES="$(diff -s test_s21_grep.log test_sys_grep.log)"
    (( COUNTER++ ))
    if [ "$DIFF_RES" == "Files test_s21_grep.log and test_sys_grep.log are identical" ]
    then
      (( SUCCESS++ ))
      echo "\033[31m$FAIL\033[0m/\033[32m$SUCCESS\033[0m/$COUNTER \033[32msuccess\033[0m grep $t"
    else
      (( FAIL++ ))
      echo "\033[31m$FAIL\033[0m/\033[32m$SUCCESS\033[0m/$COUNTER \033[31mfail\033[0m grep $t"
    fi
    rm test_s21_grep.log test_sys_grep.log
}

for i in "${tests[@]}"
do
    var="-"
    testing $i
done


echo "\033[31mFAIL: $FAIL\033[0m"
echo "\033[32mSUCCESS: $SUCCESS\033[0m"
echo "ALL: $COUNTER"
