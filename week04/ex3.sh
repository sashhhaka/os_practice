#!/bin/bash

gcc ex3.c -o ex3

./ex3 3 &
echo "n=3"

echo "1st pstree:"
pstree
echo "2nd pstree:"
pstree
echo "3rd pstree:"
pstree

./ex3 5 &
echo "n=5"

echo "1st pstree:"
pstree
echo "2nd pstree:"
pstree
echo "3rd pstree:"
pstree
echo "4th pstree:"
pstree
echo "5th pstree:"
pstree
echo "6th pstree:"
pstree

