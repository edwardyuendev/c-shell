CC = gcc
CFLAGS = -g -Wall -Werror

default: all

simple_shell.o: simple_shell.c
	$(CC) $(CFLAGS) -c simple_shell.c -o simple_shell.o

all: simple_shell.o
	gcc simple_shell.o -o simple_shell

clean:
	-rm -f simple_shell.o
	-rm -f simple_shell
