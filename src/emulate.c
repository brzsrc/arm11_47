#include <stdlib.h>
#include <stdio.h>
#include <assert.h>


int *readFile(char *filename) {
  int *objectCode = calloc(16384, sizeof(int)); //Creates a pointer with allocated space of 64KB
  int i = 0;
  FILE *ptr = fopen(filename, "rb"); //Opens file "filename" to be read as a binary file
  assert(ptr != NULL); //Checks the file has been read
  
  while (!feof(ptr)) {
    fread(objectCode + i, 4, 1, ptr); //Stores the binary data in the files into the array objectCode
    i++;
  }
  
  fclose(ptr); //Closes file "filename"
  
  return objectCode;
}

int fetch(int *objectcode, int programCounter) { 
  return objectcode[programCounter];
}

void initialiseRegisters(int *registers) {
  for (int i = 0; i < 17; i++) {
      registers[i] = 0;
    }
}

int main(int argc, char **argv) {
  
  int registers[17];
  int *objectcode;
  
  assert(argc == 2);
  
  initialiseRegisters(&registers[0]);
  objectcode = readFile(argv[1]);
  
  printf("%d\n", fetch(objectcode, 0));
  
  free(objectcode);
  
  return EXIT_SUCCESS;
}
