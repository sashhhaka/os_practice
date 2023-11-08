#!/bin/bash

pages=$1
frames=$2
algo=$3
ref_string=${@:4}

gcc mmu.c -o mmu
gcc pager.c -o pager

mkdir -p /tmp/ex2/
touch /tmp/ex2/pagetable.txt

./pager $pages $frames $algo > pager.txt &

pagerPID=$(pidof -s pager)
echo "Pager PID: $pagerPID"

./mmu $pages "$ref_string" "$pagerPID" > mmu.txt

rm -rf /tmp/ex2/
rm mmu
rm pager


