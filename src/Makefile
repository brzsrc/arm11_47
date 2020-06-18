CC      = gcc
CFLAGS  = -Wall -g -std=c99 -Werror -pedantic
EMULATEFILES = emulate.c fetch.c decode.c execute.c instrType.c
ASSEMBLEFILES = assemble.c assemble_utils.c
#.PHONY: all clean

all: emulate assemble

assemble: assemble.o assemble_utils.o
		$(CC) $(CFLAGS) $(ASSEMBLEFILES) -o assemble

emulate: emulate.o fetch.o decode.o execute.o instrType.o
	$(CC) $(CFLAGS) $(EMULATEFILES) -o emulate

clean:
	rm -f $(wildcard *.o)
	rm -f emulate
	rm -f assemble
