#!/bin/bash

compile_and_run() {
    gcc -o ex1 ex1.c
    ./ex1 &
}

get_password() {
    pid=$(cat /tmp/ex1.pid)
    str=$(grep heap /proc/"$pid"/maps)

    start_address="0x${str:0:12}"
    end_address="0x${str:13:12}"
    length=$((end_address - start_address))

    str=$(sudo xxd -s "$start_address" -l "$length" /proc/"$pid"/mem | grep pass)
    pass="${str#*:}"
    pass="${pass#*:}"

    if [[ -z $pass ]]; then
        echo "Error while reading the password"
        sudo kill -9 "$pid"
        exit 1
    fi

    echo "Password was found at address: $start_address"
    echo "Password: $pass"
}

cleanup() {
    sudo kill -9 "$pid"
    rm ex1
}

main() {
    compile_and_run
    get_password
    cleanup
}

main
