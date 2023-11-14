#!/bin/bash

bash gen.sh 10 ex1.txt

ln ex1.txt ex11.txt
ln ex1.txt ex12.txt

cat ex1.txt
cat ex11.txt
cat ex12.txt


ls -i ex1.txt ex11.txt ex12.txt > output.txt

du ex1.txt

ln ex1.txt ex13.txt
mv ex13.txt /tmp

find ./ -inum $(ls -i ex1.txt | cut -d' ' -f1)

find / -inum $(ls -i ex1.txt | cut -d' ' -f1) 2>/dev/null

ls -l ex1.txt

find / -inum $(ls -i ex1.txt | cut -d' ' -f1) -exec rm {} \;

