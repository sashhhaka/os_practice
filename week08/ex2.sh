#!/bin/bash

echo "Starting simulation..."

# example: ./ex2.sh 4 2 R0 R1 W1 R0 R2 W2 R0 R3 W2
pages=$1
frames=$2
# parse ref_string from cmd till the end
ref_string=${@:3}

# Compile mmu.c
gcc mmu.c -o mmu

# Compile pager.c
gcc pager.c -o pager

# Create directory for mapped file
mkdir -p /tmp/ex2/
touch /tmp/ex2/pagetable.txt

# Run pager process in background
./pager $pages $frames > pager.txt &

pagerPID=$(pidof -s pager)
echo "Pager PID: $pagerPID"

# Run MMU process
./mmu 4 "$ref_string" "$pagerPID" > mmu.txt


# Cleanup
rm -rf /tmp/ex2/
rm mmu
rm pager


echo "Output of both programs is is pager.txt and mmu.txt"
echo "Simulation finished."