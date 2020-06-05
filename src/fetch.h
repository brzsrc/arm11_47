#ifndef FETCH_H
#define FETCH_H

void readFile(char *filename, unsigned int *memory); //Reads the entire binary file and copies it into memory

unsigned int fetch(unsigned int *objectcode, unsigned int programCounter); //Fetches 4 bytes worth of data from the memory depending on where the program counter is
#endif
