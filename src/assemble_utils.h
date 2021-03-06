#ifndef ASSEMBLE_UTILS_H
#define ASSEMBLE_UTILS_H
#include <stdio.h>

static const int undefined = -436207600; //11100110000000000000000000010000

enum mnemonics {
	ADD, SUB, RSB, AND, EOR, ORR, MOV, TST, TEQ, CMP, MUL, MLA, LDR, STR,
	BEQ, BNE, BGE, BLT, BGT, BLE, B,
	BLEQ, BLNE, BLGE, BLLT, BLGT, BLLE, BL,	//extension instruction set
	LSL, ANDEQ, SWI, BX, SWP, SWPB, UNDEFINED	//swi, bx, swp, swpb - extension instructions

};

int findLabel(FILE *srcFile, char *label, int currentAddress);

int getNoOfInstructions(FILE *srcFile);

void second_pass(FILE *srcFile, FILE *dstFile);

int generateBinary(char *buffer, int *storedValues, int currentAddress, int noOfInstructions, FILE *srcFile);

int findMnemonic(char *p);

int assembleDataProcessing(enum mnemonics mnemonic, char operands[6][20]);

int assembleMultiply(enum mnemonics mnemonic, char operands[6][20]);

int assembleSingleDataTransfer(enum mnemonics mnemonic, char operands[6][20], int *storedValues, int noOfInstructions, int pc);

int assembleBranch(enum mnemonics mnemonic, char operands[6][20], FILE *srcFile, int currentAddress);

int assembleBranchLink(enum mnemonics mnemonic, char operands[6][20], FILE *srcFile, int currentAddress);

int assembleSpecial(enum mnemonics mnemonic, char operands[6][20]);

int checkBit(int pos, int number);

void setBitAtPos(int pos, int value, int *number);

void setBitSequence(int pos1, int pos2, int value, int *number);

#endif
