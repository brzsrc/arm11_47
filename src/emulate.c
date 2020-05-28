#include <stdlib.h>
#include <stdio.h>
#include <assert.h>


char *readFile(char *filename) {
  char *objectCode = calloc(65536, sizeof(char)); //Creates a pointer with allocated space of 64KB
  
  FILE *ptr = fopen(filename, "rb"); //Opens file "filename" to be read as a binary file
  assert(ptr != NULL); //Checks the file has been read
  
  fread(objectCode, 4, 16384, ptr); //Stores the binary data in the files into the array objectCode
  
  fclose(ptr); //Closes file "filename"
  
  return objectCode;
}

char *fetch(char *objectcode, int programCounter) {
  char instruction[32];
  
  for (int i = 0; i < 32; i++) {
    instruction[i] = *(objectcode + (32 * programCounter));
  }
  
  char *result = instruction;
  
  return result;
}

void initialiseRegisters(char **registers) {
  for (int i = 0; i < 17; i++) {
    for (int j = 0; j < 32; j++) {
      registers[i][j] = '0';
    }
  }
}

int main(int argc, char **argv) {
  
  char **registers = calloc(17, 32 * sizeof(char));
  char *objectcode;
  
  assert(argc == 2);
  
  objectcode = readFile(argv[1]);
  
  printf("%s\n", fetch(objectcode, 0));
  
  for (int i = 0; i < 17; i++) {
    free(registers[i]);
  }
  
  free(registers);
  free(objectcode);
  
  return EXIT_SUCCESS;
}
