#!/bin/bash

# Compile worker.c
gcc worker.c -o worker


# Compile scheduler.c
gcc scheduler.c -o scheduler


# Run the scheduler program with data.txt as a command-line argument
./scheduler data.txt

# Clean up by removing the compiled worker and scheduler executables
rm worker
rm scheduler
