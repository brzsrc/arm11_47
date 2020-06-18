#include <stdio.h>
#include <assert.h>
#include "fetch.h"

void readFile(char *filename, unsigned int *memory) {
    //Creates a pointer with allocated space of 64KB
    int i = 0;
    FILE *ptr = fopen(filename, "rb"); //Opens file "filename" to be read as a binary file
    assert(ptr); //Checks the file has been read

    while (!feof(ptr)) {
        fread(memory + i, 4, 1, ptr);
        //Stores the binary data in the files into the array objectCode
        i++;
    }
    fclose(ptr); //Closes file "filename"
} //loader

unsigned int fetch(unsigned int *objectcode, unsigned int programCounter) {
    return objectcode[programCounter]; //Returns an instruction at the value of programCounter
}
