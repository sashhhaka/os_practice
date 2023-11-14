#!/bin/bash

ls -i ex1 
#2496503 ex1
stat ex1 
#264986 ex1
#  File: ex1
#  Size: 21448           Blocks: 48         IO Block: 4096   regular file
#Device: 10308h/66312d   Inode: 264986      Links: 1
#Access: (0775/-rwxrwxr-x)  Uid: ( 1000/    sshk)   Gid: ( 1000/    sshk)
#Access: 2023-11-14 23:51:02.991263805 +0300
#Modify: 2023-11-14 23:51:02.987263775 +0300
#Change: 2023-11-14 23:51:02.987263775 +0300
# Birth: 2023-11-14 23:51:02.971263657 +0300
ls -l ex1
#-rwxrwxr-x 1 sshk sshk 21448 ноя 14 23:51 ex1
cp ex1 ex2
ls -i ex2
#264987 ex2
ls -l ex2
#-rwxrwxr-x 1 sshk sshk 21448 ноя 14 23:52 ex2 (number of links is 1)


# Inodes are not the same, because when you copy a file in most modern systems,
# it creates a new file with a new inode.

stat -c "%h - %n" /etc/* | grep ^3
#3 - /etc/acpi
#3 - /etc/alsa
#3 - /etc/apparmor
#3 - /etc/avahi
#3 - /etc/ca-certificates
#3 - /etc/default
#3 - /etc/emacs
#3 - /etc/firefox
#3 - /etc/fwupd
#3 - /etc/gdb
#3 - /etc/glvnd
#3 - /etc/gss
#3 - /etc/ifplugd
#3 - /etc/libblockdev
#3 - /etc/libreoffice
#3 - /etc/perl
#3 - /etc/pm
#3 - /etc/sane.d
#3 - /etc/sgml
#3 - /etc/ssh
#3 - /etc/ufw
#3 - /etc/update-manager
#The number of links = the number of hard links to the file.

