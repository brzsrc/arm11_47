#ifndef DECODE_H
#define DECODE_H

#include "macrosAndStructs.h"

void findOpcode(decoded *decodedInstr, unsigned int instruction); //Finds the opcode of a data processing instruction

void decode(unsigned int instruction, decoded *decodedInstr); //Decodes the instruction and stores it in decodedInstr

#endif
