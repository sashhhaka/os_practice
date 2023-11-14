#!/bin/bash

file /dev/* #most of them are "character special", also there is enough "block special" and "directory" and other types
file /etc/* #most of them are "ASCII text" or "directory", and ofc there are other types as well

ls -l /etc | grep ^d | wc -l #134

#"Hello World!"
file ex1.c #C source, ASCII text
gcc ex1.c -o ex1
file ex1 #ELF 64-bit LSB pie executable, x86-64, version 1 (SYSV), dynamically linked, interpreter /lib64/ld-linux-x86-64.so.2, BuildID[sha1]=5bbd48e0b49d5c70a228dfc8a588d80c167fe2db, for GNU/Linux 3.2.0, not stripped

#"Привет, мир!"
file ex1.c #C source, Unicode text, UTF-8 text	
gcc ex1.c -o ex1
file ex1 #ELF 64-bit LSB pie executable, x86-64, version 1 (SYSV), dynamically linked, interpreter /lib64/ld-linux-x86-64.so.2, BuildID[sha1]=7e87250cbaa8ffc5df13d615218d0639f1b33e7f, for GNU/Linux 3.2.0, not stripped

# We see the difference in code standarts. ASCII, as American Standart, doesn't contain Russian alphabet and contains English alphabet (as international language). In other side, Unicode contains most of World languages and it is used for Russian letters because they are not in ASCII.
