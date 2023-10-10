gcc -pthread ex2.c -o ex2

if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <threads_num>"
    exit 1
fi

threads_num=$1

./ex2 $threads_num

rm -f ex2