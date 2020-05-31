#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>

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
  return objectcode[programCounter]; //Returns an instruction at the value of programCounter
}

void initialiseRegisters(int *registers) {
  for (int i = 0; i < 17; i++) {
      registers[i] = 0; //Sets all the registers to 0
    }
}

void decode(int instruction, int programcounter) { //Decode started but not working yet
  enum instrSet{dataProcess, mul, singleData, branch};
  enum instrSet instrType;
  int bit26and27 = 134217728 + 67108864;
  
  if ((bit26and27 & instruction) == 0) {
    instrType = dataProcess;
  } else if ((bit26and27 & instruction) == 67108864) {
    instrType = singleData;
  } else if ((bit26and27 & instruction) == 134217728) {
    instrType = branch;
  }
  
  
  /*switch (instrType) {
    case dataProcess:
      decodeDataProcess(instruction, programcounter);
    break;
  }*/
}

void decodeBranch(int instruction, int pc) {
  int cond = (instruction & -268435456) / 26843546; 
  printf("%d\n", cond);
}

/*void decodeDataProcess(int instruction, int pc, char* decodedData) {
  int opcode = ((2^24 + 2^23 + 2^22 + 2^21) & instruction) / (2^21)
  switch (opcode) {
    case 0:
    break;
    case 1:
    break;
    case 2:
    break;
    case 3:
    break;
    case 4:
    break;
    case 8:
    break;
    case 9:
    break;
    case 10:
    break;
    case 12:
    break;
    case 13:
    break;
  
  }
}*/

void startCycle(int *objectcode, int *registers) {
  int pc = registers[15]; //pc takes the value of the program counter
  int fetchedInstr = NULL;
  char *decodedInstr = NULL;
  if (pc == 0) {
    fetchedInstr = fetch(objectcode, pc);
    pc = pc + 4;
  } else if (pc == 4) {
    decode(objectcode, (pc/4 - 1));
    fetch(objectcode, pc/4);
    pc = pc + 4;
  } else {
    //execute(objectcode, (pc/4 - 2));
    decode(objectcode, (pc/4 - 1));
    fetch(objectcode, pc/4);
    decode(objectcode, (pc/4 - 1));
    pc = pc + 4;
  }
}

int main(int argc, char **argv) {
  
  int registers[17];
  int *objectcode;
  char *decodedData;
  bool halt;
  
  assert(argc == 2);
  
  initialiseRegisters(&registers[0]);
  objectcode = readFile(argv[1]);
  
  while (!halt) {
  startCycle(objectCode, &registers[0]);
  }
  
  
  printf("%d\n", fetch(objectcode, 0));
  
  free(objectcode);
  
  return EXIT_SUCCESS;
}
