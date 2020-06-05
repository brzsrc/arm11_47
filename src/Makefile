CC      = gcc
CFLAGS  = -Wall -g -std=c99 -Werror -pedantic
FILES = emulate.c fetch.c decode.c execute.c instrType.c

#.PHONY: all clean

all: emulate.o fetch.o execute.o decode.o instrType.o
	$(CC) $(CFLAGS) $(FILES) -o emulate

clean:
	rm -f $(wildcard *.o)
	rm -f assemble
	rm -f emulate
