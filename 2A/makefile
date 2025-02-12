CC = gcc 
CLAGS = -Wall -g -lm
OBJ = utils.o sender.o receiver.o

all: sender receiver

sender: sender.o utils.o
  $(CC) $(CFLAGS) sender.o utils.o -o sender

receiver: receiver.o utils.o
  $(CC) $(CFLAGS) receiver.o utils.o -o receiver

utils.o: utils.c utils.h
  $(CC) $(CFLAGS) -c utils.c -o utils.o

sender.o: sender.c utils.h
  $(CC) $(CFLAGS) -c sender.c -o sender.o

receiver.o: receiver.c utils.h
  $(CC) $(CFLAGS) -c receiver.c -o receiver.o

clean:
  rm -f *.o sender receiver
