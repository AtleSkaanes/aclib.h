#!/usr/bin/env bash

ret_code=0
total_ran_ok=0
total_ran_err=0

cc_flags="-Wall -Wpedantic -Wno-gnu-statement-expression -Wno-format-pedantic -Wno-gnu-compound-literal-initializer"

for file in ./tests/*.c; do  
    if [ -f "$file" ]; then 
        clang "$file" $cc_flags -o test || continue
        echo -e "\x1b[34m# Running all tests in $file\x1b[0m"

        # output="$(./test | tee /dev/stderr ; ( exit ${PIPESTATUS[0]}))"
        output="$(./test)"
        curr_ret_code=$?

        if [ -n "$output" ]; then
            ran_ok="$(echo "$output" | sed -n "1 p")"
            total_ran_ok="$(expr "$total_ran_ok" + "$ran_ok" )"

            ran_err="$(echo "$output" | sed -n "2 p")"
            total_ran_err="$(expr "$total_ran_err" + "$ran_err" )"
        fi

        [ "$ret_code" == "0" ] && ret_code=$curr_ret_code
        rm ./test
        echo ""
    fi 
done

echo -e "Completed \x1b[32m$total_ran_ok\x1b[0m test(s)"
if [ "$total_ran_err" -gt 0 ] ; then
    echo -e "Failed \x1b[31m$total_ran_err\x1b[0m test(s)"
    exit 1
fi
exit 0

