#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "decode.h"
#include "fetch.h"
#include "execute.h"
#include <math.h>

#define NREGS 17
#define STACK_SIZE 16384 //Defined stack as ints which are 4 bytes so (64*1024)/4
#define PC 15

void bigToLittleEndian(armstate *state) {
	int temp;
	for (int j = 0; j < STACK_SIZE; j++) {
		temp = state->memory[j];
		state->memory[j] = (temp & 0xff) << 24 | (temp & 0xff00) << 8 |
		(temp & 0xff0000) >> 8 |	(temp & 0xff000000) >> 24;
	}
}

void printResult(armstate *state, int numOfInstr) {
  bigToLittleEndian(state);
  printf("Registers:\n");
  for (int i = 0; i < 13; i++) {
		if (i < 10) {
  	  printf("$%d  :%11d (0x%08x)\n", i, state->regs[i], state->regs[i]);
	  } else {
			printf("$%d :%11d (0x%08x)\n", i, state->regs[i], state->regs[i]);
		}
  }
  printf("PC  :%11d (0x%08x)\n", state->regs[PC], state->regs[PC]);
	if (state->regs[16] >= pow(2, 31)) {
    printf("CPSR:%12d (0x%08x)\n", state->regs[16], state->regs[16]);
  } else {
		printf("CPSR:%11d (0x%08x)\n", state->regs[16], state->regs[16]);
	}
  printf("Non-zero memory:\n");
	for (int i = 0; i < ((state->regs[PC] / 4) - 2); i++) {
    printf("0x%08x: 0x%08x\n", i * 4, state->memory[i]);
	}
}

void startCycle(armstate *state) {
    unsigned int *pc = &(state->regs[PC]); //pc takes the value of the program counter
    unsigned int fetched, counter = 0;
    decoded *decodedInstr = (decoded *) calloc(1, sizeof(decoded));
    bool finished = false;
    unsigned int *objectcode = state->memory;

  while(!finished) {
      if (*pc == 0) {
          fetched = fetch(objectcode, *pc/4);
          *pc += 4;
      } else if (*pc == 4) {
          decode(fetched, decodedInstr);
          fetched = fetch(objectcode, *pc/4);
          *pc += 4;
      } else {
        execute(decodedInstr, state);
        if (decodedInstr->type == branch && do_next_instruction(decodedInstr,
					state)) {
          fetched = fetch(objectcode, *pc/4);
          *pc += 4;
        }
        decode(fetched, decodedInstr);
        fetched = fetch(objectcode, *pc/4);
        *pc += 4;
        if (decodedInstr->condition == 0 && decodedInstr->bit0to25 == 0 &&
					decodedInstr->type == and){
          finished = true;
					counter -= 2;
        }
      }
      state->regs[16] = state->n << 31 | state->z << 30 | state->c << 29 |
			state->v << 28;
      counter++;
    }

    printResult(state, counter);

    free(decodedInstr);
} //pipeline

int main(int argc, char **argv) {

    //unsigned int registers[NREGS];
    armstate *state = (armstate *) calloc(1, sizeof(armstate));

    armstate_init(state); // &??????????

    assert(argc == 2);
    readFile(argv[1], state->memory);

    startCycle(state); //???????

    free(state->memory);
    free(state);
    return EXIT_SUCCESS;
}
