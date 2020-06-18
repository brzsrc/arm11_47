#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "assemble_utils.h"
#include "macrosAndStructs.h"

static const char *mnemonicsList[] = {
	"add", "sub", "rsb", "and", "eor", "orr", "mov", "tst", "teq", "cmp", "mul",
	"mla", "ldr", "str", "beq", "bne", "bge", "blt", "bgt", "ble", "b", "lsl", "andeq"
};

int findLabel(FILE *srcFile, char *label, int currentAddress) {
	int i = 0;
	char *buffer = calloc(MAX_LENGTH_OF_LINE, sizeof(char));
	rewind(srcFile);
	label[strlen(label) - 1] = '\0';
	char *colon = ":\n";
	strcat(label, colon);
	while (fgets(buffer, MAX_LENGTH_OF_LINE, srcFile)) {
		if (strcmp(buffer, label) == 0) {
			return i - currentAddress - PC_OFFSET;
		}
		if (buffer[strlen(buffer) - 2] != ':') {
		  i += 4;
		}
	}
	return 0;
}

int getNoOfInstructions(FILE *srcFile) {
	int result = 0;
	char *buffer = calloc(MAX_LENGTH_OF_LINE, sizeof(char));
	while (fgets(buffer, MAX_LENGTH_OF_LINE, srcFile)) {
		int len = strlen(buffer);
		if (buffer[len - 2] != ':') {
			result++;
		}
	}
	return result;
}

void second_pass(FILE *srcFile, FILE *dstFile) {
	int currentAddress = 0;
	int noOfInstructions = getNoOfInstructions(srcFile);
	rewind(srcFile);
	char *buffer = calloc(MAX_LENGTH_OF_LINE, sizeof(char));
	int *storedValues = calloc(MAX_LENGTH_OF_LINE, sizeof(char));
	memset(buffer, 0, sizeof(char) * sizeof(buffer));
	while (fgets(buffer, MAX_LENGTH_OF_LINE, srcFile)) {
		int len = strlen(buffer);
		if (buffer[len - 2] != ':' && *buffer != '\n') {
		  int instruction = 0;
		  instruction = generateBinary(buffer, storedValues, currentAddress, noOfInstructions, srcFile);
		  fwrite(&instruction, sizeof(int), 1, dstFile);
		  currentAddress += 4;
	  }
		memset(buffer, 0, sizeof(char) * sizeof(buffer));
	}
	int i = 0;
	while (storedValues[i]) {
		fwrite(&storedValues[i], sizeof(int), 1, dstFile);
		i++;
	}
	free(storedValues);
	free(buffer);
}



void setBitAtPos(int pos, int value, int *number) {
	*number |= (value << pos);
}

int generateBinary(char *buffer, int *storedValues, int currentAddress, int noOfInstructions, FILE *srcFile) {
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
		return assembleSingleDataTransfer(mnemonic, operands, storedValues, noOfInstructions, currentAddress);
	}
	if (mnemonic < LSLOP) {
		return assembleBranch(mnemonic, operands, srcFile, currentAddress);
	}
	return assembleSpecial(mnemonic, operands);
}

int findMnemonic(char *p) {
    int i;
    for (i = 0; i < NUM_OF_MNENMONICS; i++) {
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
	if (result < MAX_LDR_USED_AS_MOV) {
		return result;
	} else {
		int x = MAX_NO_OF_ROTATIONS;
		while(result > 0xFF || ((result & 3) == 0)) {
			result = (result >> ONE_ROTATION);
			x--;
		}
		result |= (x << ROTATE_BIT);
		return result;
	}
}

int isImmediate(char operands[20]) {
	if (operands[0] == '#') {
		return 1;
	}
	return 0;
}

int assembleDataProcessing(enum mnemonics mnemonic, char operands[6][20]) {
	int result = 0, opCode, cond = COND_AL; //14 = 1110
	setBitAtPos(COND_BIT, cond, &result);
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
		setBitAtPos(DATA_PROCESS_OPCODE_BIT, opCode, &result);
		setBitAtPos(DATA_PROCESS_RN_BIT, rn, &result);
		setBitAtPos(DATA_PROCESS_RD_BIT, rd, &result);
		int operand2 = getOperand2(operands[2]);
		int immediate = isImmediate(operands[2]);
		setBitAtPos(DATA_PROCESS_I_BIT, immediate, &result);
		setBitAtPos(0, operand2, &result);
		return result;
	}
	if (mnemonic == MOV) {
		int rd = atoi(operands[0] + 1);
		opCode = 13; //1101
		setBitAtPos(DATA_PROCESS_OPCODE_BIT, opCode, &result);
		setBitAtPos(DATA_PROCESS_RD_BIT, rd, &result);
		int operand2 = getOperand2(operands[1]);
		int leftShift = 0;
		if (!strcmp(operands[2], "lsl")) {
			leftShift = getOperand2(operands[3]);
		}
		int immediate = isImmediate(operands[1]);
		setBitAtPos(DATA_PROCESS_I_BIT, immediate, &result);
		setBitAtPos(0, operand2, &result);
		setBitAtPos(7, leftShift, &result);
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
	int rn = atoi(operands[0] + 1);
	int operand2 = getOperand2(operands[1]);
	int immediate = isImmediate(operands[1]);
	setBitAtPos(DATA_PROCESS_I_BIT, immediate, &result);
	setBitAtPos(0, operand2, &result);
	setBitAtPos(DATA_PROCESS_S_BIT, 1, &result);
	setBitAtPos(DATA_PROCESS_OPCODE_BIT, opCode, &result);
	setBitAtPos(DATA_PROCESS_RN_BIT, rn, &result);
	return result;
}

int assembleMultiply(enum mnemonics mnemonic, char operands[6][20]) {
	int result = 0, cond = COND_AL, mask = 9;
	int rd = atoi(operands[0] + 1), rs = atoi(operands[2] + 1);
	int rm = atoi(operands[1] + 1); //1110, 1001
	setBitAtPos(COND_BIT, cond, &result);
	setBitAtPos(MUL_RD_BIT, rd, &result);
	setBitAtPos(MUL_RS_BIT, rs, &result);
	setBitAtPos(4, mask, &result);
	setBitAtPos(0, rm, &result);
	if (mnemonic == MLA) {
		int rn = atoi(operands[3] + 1);
		setBitAtPos(MUL_RN_BIT, rn, &result);
		setBitAtPos(MUL_ACC_BIT, 1, &result);
	}
	return result;
}

int assembleSingleDataTransfer(enum mnemonics mnemonic, char operands[6][20], int *storedValues, int noOfInstructions, int pc) {
	int result = 0, cond = COND_AL, offset; //1110
	int rd = atoi(operands[0] + 1);
	setBitAtPos(SINGLEDATA_RD_BIT, rd, &result);
	setBitAtPos(COND_BIT, cond, &result);
	if (mnemonic == LDR) {
		if (operands[1][0] == '=') {
			int operand2 = (int) strtol(operands[1] + 1, NULL, 0);
			if (operand2 < (1 << 12)) {
				setBitAtPos(DATA_PROCESS_I_BIT, 1, &result);
				setBitAtPos(DATA_PROCESS_OPCODE_BIT, 13, &result);
			  setBitAtPos(0, operand2, &result);
				return result;
			} else {
				//Write the value to the end of the binary file
				int i = 0;
				while(storedValues[i]) {
					i++;
				}
				storedValues[i] = operand2;
				offset = (noOfInstructions - (pc / 4) + i - 2) * 4 ;
				setBitAtPos(0, offset, &result);
				setBitAtPos(SINGLEDATA_LOAD_BIT, 1, &result);
				setBitAtPos(SINGLEDATA_P_BIT, 1, &result);
				setBitAtPos(26, 1, &result);
				setBitAtPos(SINGLEDATA_LOAD_BIT, 1, &result);
				setBitAtPos(SINGLEDATA_UP_BIT, 1, &result);
				setBitAtPos(SINGLEDATA_RN_BIT, 15, &result);
				return result;
			}
		}
	}
		setBitAtPos(26, 1, &result);
		if (mnemonic == LDR) {
		  setBitAtPos(SINGLEDATA_LOAD_BIT, 1, &result);
		}
		int rn = atoi(operands[1] + 2);
		setBitAtPos(SINGLEDATA_RN_BIT, rn, &result);
		if (operands[1][3] == ']' && !operands[2][0]) {
			offset = 0;
			setBitAtPos(SINGLEDATA_P_BIT, 1, &result);
			setBitAtPos(SINGLEDATA_UP_BIT, 1, &result);
		} else {
			if (operands[2][0] == '#') {
				setBitAtPos(SINGLEDATA_P_BIT, 1, &result);
				if (operands[2][2] == 'x') {
					offset = (int) strtol(operands[2] + 1, NULL, 0); //Converts a hex string into an integer
					setBitAtPos(SINGLEDATA_UP_BIT, 1, &result);
				} else if (operands[2][3] == 'x' && operands[2][1] == '-') {
					offset = (int) strtol(operands[2] + 2, NULL, 0);
				} else {
					offset = atoi(operands[2] + 1);
					setBitAtPos(SINGLEDATA_UP_BIT, 1, &result);
				}
				setBitAtPos(0, offset, &result);
			} else {
				offset = atoi(operands[2] + 1);
				if (operands[2][strlen(operands[2]) - 2] == ']') {
					setBitAtPos(SINGLEDATA_P_BIT, 1, &result);
				}
				setBitAtPos(SINGLEDATA_UP_BIT, 1, &result);
				setBitAtPos(SINGLEDATA_I_BIT, 1, &result);
				setBitAtPos(0, offset, &result);
			}
		}
  return result;
}

int assembleBranch(enum mnemonics mnemonic, char operands[6][20], FILE *srcFile, int currentAddress) {
	int result = 0, cond = COND_AL, mask = 5, offset; //1110, 101
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
	setBitAtPos(COND_BIT, cond, &result);
	setBitAtPos(25, mask, &result);
	int temp = ftell(srcFile);
	offset = findLabel(srcFile, operands[0], currentAddress);
	if (offset < 0) {
		offset = offset >> 2;
		offset &= 0x00ffffff; //Converts offset into signed 24 bit integer
	}
  setBitAtPos(0, offset, &result);
	fseek(srcFile, temp, SEEK_SET);
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
