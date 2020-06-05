#ifndef DECODE_H
#define DECODE_H
typedef enum {and, eor, sub, rsb, add, tst, teq, cmp, orr, mov, multiply, branch, singledata}type;

typedef struct decoded {
    type type;
    unsigned int condition;
    unsigned int bit0to25;
} decoded;

void findOpcode(decoded *decodedInstr, unsigned int instruction); //Finds the opcode of a data processing instruction

void decode(unsigned int instruction, decoded *decodedInstr); //Decodes the instruction and stores it in decodedInstr

#endif
