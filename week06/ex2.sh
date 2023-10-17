#!/bin/bash

# Compile worker.c
gcc worker.c -o worker

# Check if the compilation was successful
if [ $? -eq 0 ]; then
    echo "Worker program compiled successfully"
else
    echo "Worker program compilation failed"
    exit 1
fi

# Compile scheduler.c
gcc scheduler.c -o scheduler

# Check if the compilation was successful
if [ $? -eq 0 ]; then
    echo "Scheduler program compiled successfully"
else
    echo "Scheduler program compilation failed"
    exit 1
fi

# Run the scheduler program with data.txt as a command-line argument
./scheduler data.txt

# Clean up by removing the compiled worker and scheduler executables
rm worker
rm scheduler
