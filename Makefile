# makefile for Snake

CC=gcc
CFLAGS=
LDFLAGS=-lcurses

all: Snake

Snake: main.o
	$(CC) $(CFLAGS) main.o -o Snake $(LDFLAGS)

main.o:
	$(CC) $(CFLAGS) -c main.c $(LDFLAGS)


clean:
	rm -f *.o Snake *.stackdump
