CC = gcc
CFLAGS= -Wall -Wextra -g

OBJS = server.o file.o mime.o array.o

all: server

server: $(OBJS)
			gcc -o $@ $^

server.o: server.c

file.o: file.c file.h

mime.o: mime.c mime.h

array.o: array.c array.h

clean:
	rm -f $(OBJS)
	rm -f server
	rm core

