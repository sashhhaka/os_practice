#!/bin/bash

gcc -pthread ex3.c -o ex3

n=10000000
m_values=(1 2 4 10 100)

> ex3_res.txt

echo "m   Execution Time (seconds)" >> ex3_res.txt

for m in "${m_values[@]}"; do
    echo "Running with m = $m..."
    execution_time=$( { time -p ./ex3 "$n" "$m"; } 2>&1 | grep real | awk '{print $2}')
    echo "$m   $execution_time" >> ex3_res.txt
    ./ex3 "$n" "$m"
done

echo "Results are stored in ex3_res.txt"

rm -f ex3