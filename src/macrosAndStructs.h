#ifndef MACROS_H
#define MACROS_H

#define NREGS 17
#define MEMORY_SIZE_BYTES 65535
#define STACK_SIZE 16384 //Defined stack as ints which are 4 bytes so (64*1024)/4
#define PC 15
#define WORDLENGTH 4
#define SINGLEDATA_I_BIT 25
#define SINGLEDATA_P_BIT 24
#define SINGLEDATA_UP_BIT 23
#define SINGLEDATA_LOAD_BIT 20
#define SINGLEDATA_RN_BIT 16
#define SINGLEDATA_RD_BIT 12
#define DATA_PROCESS_I_BIT 25
#define DATA_PROCESS_S_BIT 20
#define DATA_PROCESS_RN_BIT 16
#define DATA_PROCESS_RD_BIT 12
#define DATA_PROCESS_OPCODE_BIT 21
#define MUL_ACC_BIT 21
#define MUL_S_BIT 20
#define MUL_RN_BIT 12
#define MUL_RD_BIT 16
#define MUL_RS_BIT 8
#define COND_AL 14
#define COND_LE 13
#define COND_GT 12
#define COND_LT 11
#define COND_GE 10
#define COND_NE 1
#define COND_EQ 0
#define SHIFT_OP 4
#define SHIFT_TYPE 5
#define COND_BIT 28
#define MAX_LENGTH_OF_LINE 511
#define PC_OFFSET 8
#define NUM_OF_MNENMONICS 35
#define MAX_LDR_USED_AS_MOV 256
#define MAX_NO_OF_ROTATIONS 16
#define ONE_ROTATION 2
#define ROTATE_BIT 8
#define INRANGE(x) (x < 0xc4653601) && (x >= 0x80000000)

typedef enum {and, eor, sub, rsb, add, tst, teq, cmp, orr, mov, multiply, branch, singledata}type;

typedef struct decoded {
    type type;
    unsigned int condition;
    unsigned int bit0to25;
} decoded;

typedef struct arm_state {
    unsigned int regs[NREGS];
    unsigned int *memory; // *objectcode
    int n, z, c, v;
} armstate;

typedef struct decoded_data_processing {
    type type;
    unsigned int rn, rd, operand2, s_bit;
    unsigned int carry_out, val_reg_last12bits;
} decoded_dp;

#endif
