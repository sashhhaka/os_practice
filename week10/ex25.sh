#!/bin/bash

touch ex5.txt

chmod a-w ex5.txt

chmod uo+rwx ex5.txt

chmod g=u ex5.txt

#660 means read and write for user and group, no permissions for others
#775 means read, write and execute for user and group, read and execute for others
#777 means read, write and execute for user, group and others
