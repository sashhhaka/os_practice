#!/bin/bash

# Check for the correct number of arguments
if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <num_subscribers>"
    exit 1
fi

num_subscribers=$1

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
    gnome-terminal --title="Subscriber $i" -- bash -c "./subscriber $i; exec bash"
done

# Wait for the publisher to finish (you may need to manually stop it)
echo "The publisher is running. Press Enter to stop it..."
read
# close all the terminals
killall gnome-terminal
# Check if all subscribers received the message
received_messages=$(find /tmp/ex1/ -type f | wc -l)
if [ "$received_messages" -eq "$num_subscribers" ]; then
    echo "All subscribers received the message."
else
    echo "Not all subscribers received the message."
fi

# Explain why we need to create n named pipes
echo "We create n named pipes for n subscribers because each subscriber needs its own communication channel (pipe) to receive messages. Named pipes provide a way to establish a dedicated communication path between the publisher and each subscriber. Without separate pipes, messages could get mixed up or lost when multiple subscribers try to read from the same pipe simultaneously." > ex1.txt

# Clean up named pipes and the directory
for ((i = 1; i <= num_subscribers; i++)); do
    rm -f /tmp/ex1/s$i
done

# Remove the directory if it's empty
rmdir /tmp/ex1 2>/dev/null

