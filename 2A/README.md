1. gcc sender.c utils.c -o sender
2. gcc receiver.c utils.c -o reciever
3. taskset -c 0 ./receiver
4. taskset -c 1 ./sender
