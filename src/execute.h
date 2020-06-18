#ifndef EXECUTE_H
#define EXECUTE_H

#include "decode.h"
#include "macrosAndStructs.h"

void armstate_init(armstate *state); //Initialises the values of the arm state

int do_next_instruction(decoded *decodedInstr, armstate *state); //Returns a 0 or 1 value whether the next instruction can take place

//Operand2 is a register
void val_reg_last12bits(decoded *decodedInstr, unsigned int *operand2, armstate *state); //Stores the value of the register from the given parameters

// the carry out of shift op
unsigned int shift_carry_out(decoded_dp *decodedDp, decoded *decodedInstr, armstate *state); //Returns the value of the carry out

void decode_data_processing(decoded *decodedInstr, decoded_dp *decodedDp, armstate *state); //Finds the values of each element in the instruction if the decoded instruction is a data process

void execute(decoded *decodedInstr, armstate *state); //Executes the instruction

#endif
