#!/bin/bash

rm -rf ./tmp 
ln -s ./tmp tmp1

ls -li

mkdir ./tmp

ls -li
#The difference is just the presence of a new directory (./tmp). It happens because when we create a soft link to not existing directory, it will be created before the creation of a directory and will have the same inode as after creating of a directory.

bash gen.sh 10 ex1.txt
mv ex1.txt ./tmp
ls -li ./tmp1

ln -s $(pwd)/tmp tmp1/tmp2

bash gen.sh 10 ex1.txt
mv ex1.txt ./tmp1/tmp2

ls -R ./tmp1
cd ./tmp1/tmp2/tmp2/tmp2/...
#bash: cd: ./tmp1/tmp2/tmp2/tmp2/...: No such file or directory
#It happend because of symbolic loop to itself, which creates symbolic link loop and works indefinitely, because of that prints error

rm -rf ./tmp
ls -R
#We see that symbolic link still exists, but points to a non-existing location.

rm tmp1
