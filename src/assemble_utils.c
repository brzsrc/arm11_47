#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "assemble_utils.h"
#include "macrosAndStructs.h"

static const char *mnemonicsList[] = {
	"add", "sub", "rsb", "and", "eor", "orr", "mov", "tst", "teq", "cmp", "mul", "mla", "ldr", "str",
	"beq", "bne", "bge", "blt", "bgt", "ble", "b",
	"bleq", "blne", "blge", "bllt", "blgt", "blle", "bl",  //extension instruction set
	"lsl", "andeq", "swi", "bx", "swp", "swpb"  //swi, bx, swp, swpb - extension instructions
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
	if (mnemonic == -1) {
        return UNDEFINED;
	}
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
	if (mnemonic < BLEQ) {
		return assembleBranch(mnemonic, operands, srcFile, currentAddress);
		}
	if (mnemonic < LSL) {
		return assembleBranchLink(mnemonic, operands, srcFile, currentAddress);
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
	int result = 0, opCode, cond = COND_AL, rn = 0, rd = 0, operand2 = 0, immediate = 0, s_bit = 0; //14 = 1110
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
				printf("Invalid mnemonic");
		}
		rd = atoi(operands[0] + 1);
		rn = atoi(operands[1] + 1);
		operand2 = getOperand2(operands[2]);
		immediate = isImmediate(operands[2]);
		int leftShift = 0, rightShift = 0;
		if (!strcmp(operands[3], "lsl")) {
			leftShift = getOperand2(operands[4]);
			setBitAtPos(8, leftShift, &result);
		}
		if (!strcmp(operands[3], "lsr")) {
			rightShift = getOperand2(operands[4]);
			setBitAtPos(8, rightShift, &result);
			setBitAtPos(4, 3, &result);
		}
	}
	if (mnemonic == MOV) {
		rd = atoi(operands[0] + 1);
		opCode = 13; //1101
		operand2 = getOperand2(operands[1]);
		int leftShift = 0;
		if (!strcmp(operands[2], "lsl")) {
			leftShift = getOperand2(operands[3]);
		}
		immediate = isImmediate(operands[1]);
		setBitAtPos(7, leftShift, &result);
	}
	if (mnemonic >= TST && mnemonic <= CMP) {
		switch (mnemonic) {
			case TST:
				opCode = 8; //1000
				rn = atoi(operands[0] + 1);
			  operand2 = getOperand2(operands[1]);
			  immediate = isImmediate(operands[1]);
				s_bit = 1;
				break;
			case TEQ:
				opCode = 9;  //1001
				rn = atoi(operands[0] + 1);
			  operand2 = getOperand2(operands[1]);
			  immediate = isImmediate(operands[1]);
				s_bit = 1;
				break;
			case CMP:
				opCode = 10; //1010
				rn = atoi(operands[0] + 1);
			  operand2 = getOperand2(operands[1]);
			  immediate = isImmediate(operands[1]);
				s_bit = 1;
				break;
			default:
				printf("Invalid mnemonic");
		}
  }
	setBitAtPos(DATA_PROCESS_I_BIT, immediate, &result);
	setBitAtPos(0, operand2, &result);
	setBitAtPos(DATA_PROCESS_S_BIT, s_bit, &result);
	setBitAtPos(DATA_PROCESS_OPCODE_BIT, opCode, &result);
	setBitAtPos(DATA_PROCESS_RN_BIT, rn, &result);
	setBitAtPos(DATA_PROCESS_RD_BIT, rd, &result);
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
	int result = 0, cond = COND_AL, offset = 0, load_bit = 0, p_bit = 0, mask = 1; //1110
	int rd = atoi(operands[0] + 1), rn = 0, immediate = 0, up_bit = 0;
	if (mnemonic == LDR) {
		load_bit = 1;
	}
	if (operands[1][0] == '=') {
		int operand2 = (int) strtol(operands[1] + 1, NULL, 0);
		if (operand2 < (1 << 12)) { //HAS FORM: ldr rd, =0x04
			mnemonic = MOV;
			operands[1][0] = '#';
			return assembleDataProcessing(mnemonic, operands); //Returns the result as a mov, data processing function
		} else { //HAS FORM: ldr rd, =0x100000000
			int i = 0;
			while(storedValues[i]) {
				i++;
			}
			storedValues[i] = operand2; //Stores the value into memory
			offset = (noOfInstructions - (pc / WORDLENGTH) + i - 2) * WORDLENGTH ;
			p_bit = 1;
			up_bit = 1;
			rn = 15;
		}
	}
	if (operands[1][0] == '[') {
		rn = atoi(operands[1] + 2);
	}
	if (operands[1][3] != ']') {
		p_bit = 1;
	}
	if (operands[1][3] == ']' && !operands[2][0]) { //HAS FORM: ldr rd, [rn]
		offset = 0;
		p_bit = 1;
	  up_bit = 1;
	}
	if (operands[2][0] == 'r') {
		up_bit = 1;
		immediate = 1;
		offset = atoi(operands[2] + 1);
	}
  if (operands[2][0] == '#') { //HAS FORM ldr rd, [rn,#1]
		up_bit = 1;
		offset = atoi(operands[2] + 1);
	}
	if (operands[2][2] == 'x') {
		offset = (int) strtol(operands[2] + 1, NULL, 0); //Converts a hex string into an integer
	}
	if (operands[2][3] == 'x' && operands[2][1] == '-') {
		offset = (int) strtol(operands[2] + 2, NULL, 0);
		up_bit = 0;
	}
  if (operands[1][3] == ']' && operands[2][0]) { //HAS FORM: ldr rd, [rn], #1
		offset = atoi(operands[2] + 1);
		up_bit = 1;
	}
	if (!strcmp(operands[3], "lsr")) {
		setBitAtPos(5, 1, &result);
		if (operands[4][0] == '#') {
			setBitAtPos(7, atoi(operands[4] + 1), &result);
		} else {
			setBitAtPos(4, 1, & result);
			setBitAtPos(8, atoi(operands[4] + 1), &result);
		}
	}
		setBitAtPos(COND_BIT, cond, &result);
		setBitAtPos(0, offset, &result);
		setBitAtPos(SINGLEDATA_LOAD_BIT, load_bit, &result);
		setBitAtPos(SINGLEDATA_P_BIT, p_bit, &result);
		setBitAtPos(26, mask, &result);
		setBitAtPos(SINGLEDATA_UP_BIT, up_bit, &result);
		setBitAtPos(SINGLEDATA_RN_BIT, rn, &result);
		setBitAtPos(SINGLEDATA_RD_BIT, rd, &result);
		setBitAtPos(SINGLEDATA_I_BIT, immediate, &result);
  return result;
}

int assembleBranch(enum mnemonics mnemonic, char operands[6][20], FILE *srcFile, int currentAddress) {
	int result = 0, cond = COND_AL, mask = 5, offset; //1110, 101
	if (mnemonic >= BEQ && mnemonic <= B) {
		switch (mnemonic) {
			case BEQ:
				cond = COND_EQ;  //0000
				break;
			case BNE:
				cond = COND_NE;  //0001
				break;
			case BGE:
				cond = COND_GE; //1010
				break;
			case BLT:
				cond = COND_LT; //1011
				break;
			case BGT:
				cond = COND_GT; //1100
				break;
			case BLE:
				cond = COND_LE; //1101
				break;
			case B:
				cond = COND_AL; //1110
				break;
			default:
				printf("Invalid mnemonic");
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

int assembleBranchLink(enum mnemonics mnemonic, char operands[6][20], FILE *srcFile, int currentAddress) {	//extension instruction set
	int result = 0;
	if (mnemonic >= BLEQ && mnemonic <= BL) {
			switch (mnemonic) {
				case BLEQ:
					result = assembleBranch(BEQ, operands, srcFile, currentAddress);
					break;
				case BLNE:
					result = assembleBranch(BNE, operands, srcFile, currentAddress);
					break;
				case BLGE:
					result = assembleBranch(BGE, operands, srcFile, currentAddress);
					break;
				case BLLT:
					result = assembleBranch(BLT, operands, srcFile, currentAddress);
					break;
				case BLGT:
					result = assembleBranch(BGT, operands, srcFile, currentAddress);
					break;
				case BLLE:
					result = assembleBranch(BLE, operands, srcFile, currentAddress);
					break;
				case BL:
					result = assembleBranch(B, operands, srcFile, currentAddress);
					break;
				default:
                    printf("Invalid mnemonic");
			}
		}
	setBitAtPos(24, 1, &result);
	return result;
}

int assembleSpecial(enum mnemonics mnemonic, char operands[6][20]) {
	int result = 0, cond = COND_AL, mask, rn;
	switch (mnemonic) {
		case SWI:	//extension instruction
			mask = 15; // 1111
			setBitAtPos(COND_BIT, cond, &result);
			setBitAtPos(24, mask, &result);
			return result;
		case BX:	//extension instruction
			mask = 1245169, rn = atoi(operands[0] + 1); //100101111111111110001
			setBitAtPos(COND_BIT, cond, &result);
			setBitAtPos(4, mask, &result);
			setBitAtPos(0, rn, &result);
			return result;
		case LSL:
			strcpy(operands[3], operands[1]);
			strcpy(operands[1], operands[0]);
			strcpy(operands[2], "lsl");
			return assembleDataProcessing(MOV, operands);
		case ANDEQ:
			return 0;
		case SWP:	//extension instruction
			mask = 9, rn = atoi(operands[2] + 2);   //1001
			int rd = atoi(operands[0] + 1), rm = atoi(operands[1] + 1);
			setBitAtPos(COND_BIT, cond, &result);
			setBitAtPos(0, rm, &result);
			setBitAtPos(12, rd, &result);
			setBitAtPos(16, rn, &result);
			setBitAtPos(4, mask, &result);
			setBitAtPos(24, 1, &result);
			return result;
		case SWPB:	//extension instruction
			result = assembleSpecial(SWP, operands);
			setBitAtPos(22, 1, &result);
			return result;
		default:
      printf("Invalid mnemonic");
			return result;
	}
}
