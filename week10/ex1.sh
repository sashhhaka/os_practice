#!/bin/bash

gcc monitor.c -o monitor
gcc ex1.c -o ex1

path=$1
# check if the directory on the path exists
if [ ! -d $path ]; then
    mkdir $path
fi

# run monitor and ex1 in separate terminals with names
gnome-terminal -- ./monitor $path
./ex1 $path

echo "Next steps are cleaning up the directory"
sleep 5


rm -r dir
rm monitor
rm ex1
rm /tmp/myfile1.txt