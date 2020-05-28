#include <stdlib.h>
#include <stdio.h>
#include <assert.h>


int *readFile(char *filename) {
  int *objectCode = calloc(16384, sizeof(int)); //Creates a pointer with allocated space of 64KB
  int i = 0;
  
  FILE *ptr = fopen(filename, "rb"); //Opens file "filename" to be read as a binary file
  assert(ptr != NULL); //Checks the file has been read
  
  while (!feof(ptr)) {
    fread((objectCode + i), 4, 1, ptr); //Stores the binary data in the files into the array
    i++;
  }
  
  fclose(ptr); //Closes file "filename"
  
  return objectCode;
}

int fetch(int *objectcode, int programCounter) {
  return objectcode[programCounter]; //Fetches one instruction from the objectcode
}

void initialiseRegisters(int *registers) {
  for (int i = 0; i < 17; i++) {
      registers[i] = 0;  //Initialises all registers to 0
  }
}

void decode(int instruction, int *registers) {
  int cond = instruction & (-268435456);
  bool dataProcessing = (instruction & 201326592 == 201326952);
  if (cond == -536870912) {  //if the condition part of instruction is always
    if (dataProcessing) {
      dataProcess(instruction, registers);
    }
  }
}

void dataProcess(int instruction, int *registers) {
  int opcode = instruction & (31457280);
  int registern = instruction & (983040);
  int registerd = instruction & (61440);
  int operand2 = instruction & (4095);
}

int main(int argc, char **argv) {
  
  int registers[17];
  int *objectcode;
  
  assert(argc == 2);
  
  initialiseRegisters(&registers[0]);
  objectcode = readFile(argv[1]);
  
  printf("%d\n", fetch(objectcode, 0));
  printf("%d\n", fetch(objectcode, 1));
  
  decode(fetch(objectcode, 0), &registers[0]);

  free(objectcode);
  
  return EXIT_SUCCESS;
}
