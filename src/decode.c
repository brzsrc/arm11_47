#include "macrosAndStructs.h"

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
    unsigned int bit26and27 = (instruction >> 26) & 3;
    decodedInstr->condition = (instruction >> COND_BIT) & 0xf;
    decodedInstr->bit0to25 = (instruction) & 0x3ffffff;

    if (bit26and27 == 0) {
        if (((instruction >> 22) & 0xf) == 0 && ((instruction >> 4) & 0xf) == 9) {
        	decodedInstr->type = multiply;
        } else {
        	findOpcode(decodedInstr, instruction);
        }
    } else if (bit26and27 == 1) {
        decodedInstr->type = singledata;
    } else if (bit26and27 == 2) {
        decodedInstr->type = branch;
    }

}
