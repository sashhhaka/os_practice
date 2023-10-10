#!/bin/bash

gcc -pthread ex4.c -o ex4

n=10000000
m_values=(1 2 4 10 100)

> ex4_res.txt

echo "m   Execution Time (seconds)" >> ex4_res.txt

for m in "${m_values[@]}"; do
    echo "Running with m = $m..."
    execution_time=$( { time -p ./ex4 "$n" "$m"; } 2>&1 | grep real | awk '{print $2}')
    echo "$m   $execution_time" >> ex4_res.txt
    ./ex4 "$n" "$m"
done

echo "Results are stored in ex4_res.txt"

rm -f ex4