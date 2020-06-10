CC      = gcc
CFLAGS  = -Wall -g -std=c99 -Werror -pedantic
EMULATEFILES = emulate.c fetch.c decode.c execute.c instrType.c
ASSEMBLEFILES = assemble.c readFile.c
#.PHONY: all clean

all: emulate assemble

emulate: emulate.o fetch.o decode.o execute.o instrType.o
	$(CC) $(CFLAGS) $(EMULATEFILES) -o emulate

assemble: assemble.o readFile.o
	$(CC) $(CFLAGS) $(ASSEMBLEFILES) -o assemble

clean:
	rm -f $(wildcard *.o)
	rm -f assemble
	rm -f emulate
