#!/bin/bash

echo "Starting simulation..."

# Compile mmu.c
gcc mmu.c -o mmu

# Compile pager.c
gcc pager.c -o pager

# Create directory for mapped file
#mkdir -p /tmp/ex2/

# Run pager process in background
gnome-terminal --title="Pager" -- ./pager 4 2

pagerPID=$(pidof -s pager)
echo $pagerPID

# Run MMU process
./mmu 4 "R0 R1 W1 R0 R2 W2 R0 R3 W2" $pagerPID

# Cleanup
#rm -rf /tmp/ex2/
rm mmu
rm pager

echo "Simulation finished."