#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "assemble_utils.h"
//return statements not included yet..
//need way to calculate Operand2..
//need to make way to read registers..
//special instructions TODO..

static const char *mnemonicsList[] = {
	"add", "sub", "rsb", "and", "eor", "orr", "mov", "tst", "teq", "cmp", "mul",
	"mla", "ldr", "str", "beq", "bne", "bge", "blt", "bgt", "ble", "b", "lsl", "andeq"
};

int checkBit(int pos, int number) {
	return (number >> pos) % pos != 0;
}

void second_pass(FILE *srcFile, FILE *dstFile) {
	int currentAddress = 0;
	char *buffer = calloc(50, sizeof(char));
	memset(buffer, 0, sizeof(char) * sizeof(buffer));
	while (fgets(buffer, 50, srcFile)) {
		int len = strlen(buffer);
		if (buffer[len - 1] == ':') {
			continue;
		}
		int instruction = 0;
		instruction = generateBinary(buffer);
		fwrite(&instruction, sizeof(int), 1, dstFile);
		currentAddress += 4;
		memset(buffer, 0, sizeof(char) * sizeof(buffer));
	}
	free(buffer);
}

void setBitAtPos(int pos, int value, int *number) {
	*number |= (value << pos);
}

int generateBinary(char *buffer) {
	//int len = strlen(buffer);
	char *sep = " ,", *token = strtok(buffer, sep), operands[6][20];
	memset(operands, 0, sizeof(operands));
	int mnemonic = findMnemonic(token), noOperands = 0;
	while (token != NULL) {
		token = strtok(NULL, sep);
		if (token) {
			strcpy(operands[noOperands], token);
			noOperands++;
		}
	}
	if (mnemonic < MUL) {
		return assembleDataProcessing(mnemonic, operands);
	}
	if (mnemonic < LDR) {
		return assembleMultiply(mnemonic, operands);
	}
	if (mnemonic < BEQ) {
		return assembleSIngleDataTransfer(mnemonic, operands);
	}
	if (mnemonic < LSLOP) {
		return assembleBranch(mnemonic, operands);
	}
	return assembleSpecial(mnemonic, operands);
}

int findMnemonic(char *p) {
    int i;
    for (i = 0; i <= 22; i++) {
        if (!strcmp(p, mnemonicsList[i])) {
            return i;
        }
    }
    return -1; //instruction not found
}

int getOperand2(char operands[20]) {
	int result = 0;
	if (operands[2] == 'x') {
		result = (int) strtol(operands + 1, NULL, 0); //Converts a hex string into an integer
	} else {
		result = atoi(operands + 1);
	}
	return result;
}

int isImmediate(char operands[20]) {
	if (operands[0] == '#') {
		return 1;
	}
	return 0;
}

int assembleDataProcessing(enum mnemonics mnemonic, char operands[6][20]) {
	int result = 0, opCode, cond = 14; //14 = 1110
	setBitAtPos(28, cond, &result);
	if (mnemonic >= ADD && mnemonic <= ORR) {
		switch (mnemonic) {
			case ADD:
				opCode = 4;  //0100
				break;
			case SUB:
				opCode = 2;  //0010
				break;
			case RSB:
				opCode = 3;  //0011
				break;
			case AND:
				opCode = 0;  //0000
				break;
			case EOR:
				opCode = 1;  //0001
				break;
			case ORR:
				opCode = 12; //1100
				break;
			default:
				perror("Invalid mnemonic");
		}
		int rd = atoi(operands[0] + 1), rn = atoi(operands[1] + 1);
		setBitAtPos(21, opCode, &result);
		setBitAtPos(16, rn, &result);
		setBitAtPos(12, rd, &result);
		int operand2 = getOperand2(operands[2]);
		int immediate = isImmediate(operands[2]);
		setBitAtPos(25, immediate, &result);
		setBitAtPos(0, operand2, &result);
		return result;
	}
	if (mnemonic == MOV) {
		int rd = atoi(operands[0] + 1);
		opCode = 13; //1101
		setBitAtPos(21, opCode, &result);
		setBitAtPos(12, rd, &result);
		int operand2 = getOperand2(operands[1]);
		int immediate = isImmediate(operands[1]);
		setBitAtPos(25, immediate, &result);
		setBitAtPos(0, operand2, &result);
		return result;
	}
	switch (mnemonic) {
		case TST:
			opCode = 8;  //1000
			break;
		case TEQ:
			opCode = 9;  //1001
			break;
		case CMP:
			opCode = 10; //1010
			break;
		default:
			perror("Invalid mnemonic");
	}
	int rn = 0;
	setBitAtPos(20, 1, &result);
	setBitAtPos(21, opCode, &result);
	setBitAtPos(16, rn, &result);
	return result;
}

int assembleMultiply(enum mnemonics mnemonic, char operands[6][20]) {
	int result = 0, cond = 14, mask = 9;
	int rd = atoi(operands[0] + 1), rs = atoi(operands[2] + 1);
	int rm = atoi(operands[1] + 1); //1110, 1001
	setBitAtPos(28, cond, &result);
	setBitAtPos(16, rd, &result);
	setBitAtPos(8, rs, &result);
	setBitAtPos(4, mask, &result);
	setBitAtPos(0, rm, &result);
	if (mnemonic == MLA) {
		int rn = atoi(operands[3] + 1);
		setBitAtPos(12, rn, &result);
		setBitAtPos(21, 1, &result);
	}
	return result;
}

int assembleSIngleDataTransfer(enum mnemonics mnemonic, char operands[6][20]) {
	int result = 0, cond = 14; //1110
	int rd = atoi(operands[0] + 1);
	setBitAtPos(28, cond, &result);
	setBitAtPos(26, 1, &result);
	if (mnemonic == LDR) {
		setBitAtPos(20, 1, &result);
	}
  return result;
}

int assembleBranch(enum mnemonics mnemonic, char operands[6][20]) {
	int result = 0, cond = 14, mask = 5; //1110, 101
	if (mnemonic >= BEQ && mnemonic <= B) {
		switch (mnemonic) {
			case BEQ:
				cond = 0;  //0000
				break;
			case BNE:
				cond = 1;  //0001
				break;
			case BGE:
				cond = 10; //1010
				break;
			case BLT:
				cond = 11; //1011
				break;
			case BGT:
				cond = 12; //1100
				break;
			case BLE:
				cond = 13; //1101
				break;
			case B:
				cond = 14; //1110
				break;
			default:
				perror("Invalid mnemonic");
		}
	}
	setBitAtPos(28, cond, &result);
	setBitAtPos(25, mask, &result);
  return result;
}

int assembleSpecial(enum mnemonics mnemonic, char operands[6][20]) {
	if (mnemonic == ANDEQ) {
		return 0;
	}
	strcpy(operands[3], operands[1]);
	strcpy(operands[1], operands[0]);
	strcpy(operands[2], "lsl");
	return assembleDataProcessing(MOV, operands);
}
