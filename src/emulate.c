#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define NREGS 17
#define STACK_SIZE 16384 //Defined stack as in5ts which are 4 bytes so (64*1024)/4
#define PC 15

typedef struct decoded {
  enum type{and, eor, sub, rsb, add, tst, teq, cmp, orr, mov, multiply, branch, singledata}type;
  unsigned int condition;
  unsigned int bit0to25;
} decoded;

/*typedef struct arm_state {
    unsigned int regs[NREGS];
    unsigned int *memory;
    int n, z, c, v;
} armstate; */

// for tests and analysis
/*struct emu_analysis_struct {
    unsigned int regs_write[NREGS];
    unsigned int regs_read[NREGS];
    int num_instructions_ex;
    int data_processing;
    int memory;
    int branches_taken;
    int branches_not_taken;
};*/

char *printenum(enum type t) { //Testing
  switch(t) {
    case and: 
      return "and";
      break;
    case eor:
      return "eor";
      break;
    case sub:
      return "sub";
      break;
    case rsb:
      return "rsb";
      break;
    case add:
      return "add";
      break;
    case tst:
      return "tst";
      break;
    case teq:
      return "cmp";
      break;
    case cmp:
      return "cmp";
      break;
    case orr:
      return "orr";
      break;
    case mov:
      return "mov";
      break;
    case multiply:
      return "mul";
      break;
    case branch:
      return "branch";
      break;
    case singledata:
      return "singledata";
      break;
  }
  return "NULL";
}

void findOpcode(decoded *decodedInstr, unsigned int instruction) {
  unsigned int opcode = (instruction >> 21) & 0xf;
  
  if (opcode == 0) {
    decodedInstr->type = and;
  } else if (opcode == 1) {
    decodedInstr->type = eor;
  } else if (opcode == 2) {
    decodedInstr->type = sub;
  } else if (opcode == 3) {
    decodedInstr->type = rsb;
  } else if (opcode == 4) {
    decodedInstr->type = add;
  } else if (opcode == 8) {
    decodedInstr->type = tst;
  } else if (opcode == 9) {
    decodedInstr->type = teq;
  } else if (opcode == 10) {
    decodedInstr->type = cmp;
  } else if (opcode == 12) {
    decodedInstr->type = orr;
  } else if (opcode == 13) {
    decodedInstr->type = mov;
  }
}

void decode(unsigned int instruction, decoded *decodedInstr) {
  unsigned int bit26and27 = (instruction >> 26) & 0b11;
  
  decodedInstr->condition = (instruction >> 28) & 0xf;
  decodedInstr->bit0to25 = (instruction) & 0x3ffffff;
  
  if (bit26and27 == 0) {
    findOpcode(decodedInstr, instruction);
  } else if (bit26and27 == 1) {
    decodedInstr->type = singledata;
  } else if (bit26and27 == 2) {
    decodedInstr->type = branch;
  }
}

unsigned int fetch(unsigned int *objectcode, unsigned int programCounter) { 
  return objectcode[programCounter]; //Returns an instruction at the value of programCounter
}

void startCycle(unsigned int *objectcode, unsigned int *registers) {
  unsigned int pc = registers[PC]; //pc takes the value of the program counter
  unsigned int fetched;
  decoded *decodedInstr = calloc(1, sizeof(decoded));
  
  for (int i = 0; i < 3; i++) {
  if (pc == 0) {
    fetched = fetch(objectcode, pc);
    pc = pc + 1;
  } else if (pc == 1) {
    decode(fetched, decodedInstr);
    fetched = fetch(objectcode, pc);
    pc = pc + 1;
  } else {
    //execute(decodedInstr);
    decode(fetched, decodedInstr);
    fetched = fetch(objectcode, pc);
    pc = pc + 1;
  }
  printf("fetched instruction: %d\n", fetched);
  printf("decoded type: %s\n", printenum(decodedInstr->type)); //Used for testing
  }
  
  free(decodedInstr);
}

unsigned int *readFile(char *filename) {
  unsigned int *objectCode = calloc(16384, sizeof(int)); 
  //Creates a pointer with allocated space of 64KB
  int i = 0;
  FILE *ptr = fopen(filename, "rb"); //Opens file "filename" to be read as a binary file
  assert(ptr != NULL); //Checks the file has been read
  
  while (!feof(ptr)) {
    fread(objectCode + i, 4, 1, ptr); 
    //Stores the binary data in the files into the array objectCode
    i++;
  }
  fclose(ptr); //Closes file "filename"
  return objectCode;
}

int main(int argc, char **argv) {

  unsigned int *objectcode = calloc(STACK_SIZE, sizeof(int));
  unsigned int registers[NREGS];
  
  for (int i = 0; i < NREGS; i++) {
    registers[i] = 0;
  }

  assert(argc == 2);
  objectcode = readFile(argv[1]);
  
  startCycle(objectcode, registers);
  
  
  
  free(objectcode);
  return EXIT_SUCCESS;
}

