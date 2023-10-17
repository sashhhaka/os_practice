#!/bin/bash

# accept file name as a command-line argument
# if no argument is provided, use data.txt as a default
if [ $# -eq 0 ]
then
    set -- "data.txt"
fi

# Check if the file exists
if [ ! -f "$1" ]
then
    echo "File $1 does not exist."
    exit 1
fi

# Compile worker.c
gcc worker.c -o worker


# Compile scheduler.c
gcc scheduler_rr.c -o scheduler_rr


# Run the scheduler program with data.txt as a command-line argument
./scheduler_rr "$1"

# Clean up by removing the compiled worker and scheduler executables
rm worker
rm scheduler_rr
