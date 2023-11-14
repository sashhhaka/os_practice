#!/bin/bash

bash gen.sh 10 ex23.txt

ln ex23.txt ex11.txt
ln ex23.txt ex12.txt

cat ex23.txt
cat ex11.txt
cat ex12.txt


ls -i ex23.txt ex11.txt ex12.txt > output.txt

du ex23.txt

ln ex23.txt ex13.txt
mv ex13.txt /tmp

find ./ -inum $(ls -i ex23.txt | cut -d' ' -f1)

find / -inum $(ls -i ex23.txt | cut -d' ' -f1) 2>/dev/null

ls -l ex1.txt

find / -inum $(ls -i ex23.txt | cut -d' ' -f1) -exec rm {} \;

