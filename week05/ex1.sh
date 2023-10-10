#!/bin/bash

# Check for the correct number of arguments
if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <num_subscribers>"
    exit 1
fi

num_subscribers=$1

# compile publisher
gcc publisher.c -o publisher
# compile n subscribers
for ((i = 1; i <= num_subscribers; i++)); do
    gcc subscriber.c -o subscriber$i
done

# Create the directory if it doesn't exist
mkdir -p /tmp/ex1

# Create n named pipes (1 for each subscriber)
for ((i = 1; i <= num_subscribers; i++)); do
    mkfifo /tmp/ex1/s$i
done

# Start the publisher in a new terminal window and print that the window is a publisher
gnome-terminal --title="Publisher" -- bash -c "./publisher $num_subscribers; exec bash"
#./publisher $num_subscribers

# Start each subscriber in a new terminal window
for ((i = 1; i <= num_subscribers; i++)); do
    gnome-terminal --title="Subscriber $i" -- bash -c "./subscriber$i $i; exec bash"
done

# Wait for the publisher to finish (you may need to manually stop it)
echo "The publisher is running. Press Enter to stop it..."
read

# Clean up named pipes and the directory
for ((i = 1; i <= num_subscribers; i++)); do
    rm -f /tmp/ex1/s$i
done

# Remove the directory if it's empty
rmdir /tmp/ex1 2>/dev/null

# Remove the executables
rm -f publisher
for ((i = 1; i <= num_subscribers; i++)); do
    rm -f subscriber$i
done

