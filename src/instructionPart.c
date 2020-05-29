// finished Single Data Transfer instructions,
// did part of the data processing instructions,
// haven't done any test yet

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define NREGS 16
#define STACK_SIZE 1024
#define PC 15

struct arm_state {
    unsigned int regs[NREGS];
    unsigned char memory_stack[STACK_SIZE];
    int n, z, k, v;
};

// for tests and analysis
struct emu_analysis_struct {
    unsigned int regs_write[NREGS];
    unsigned int regs_read[NREGS];
    int num_instructions_ex;
    int data_processing;
    int memory;
    int branches_taken;
    int branches_not_taken;
};

void set_flag(int n, int z, int k, int v, struct arm_state *state) {
    state -> n = n;
    state -> z = z;
    state -> k = k;
    state -> v = v;
}


// next_instruction_condition_check
int do_next_instruction(struct arm_state *state, struct emu_analysis_struct *analysis){
    unsigned int ni = *((unsigned int *) state -> regs[PC]);
    unsigned int cond = (ni >> 28) & 0xF;
    int run_command;
    if ((cond == 0) && (state->z == 1 )) { // cond in decimal?
        run_command = 1;
    } else if ((cond == 1) && (state->z == 0)) {
        run_command = 1;
    } else if ((cond == 10) && (state->n == state->v)) {
        run_command = 1;
    } else if ((cond == 11) && (state->n != state->v)) {
        run_command = 1;
    } else if (cond == 12 && state->z == 0 && state->n == state->v) {
        run_command = 1;
    } else if (cond == 13 && (state->z == 1 || state->n != state->v)) {
        run_command = 1;
    } else if (cond == 14){
        run_command = 1;
    } else {
        run_command = 0;
    }
    return run_command;
}


//Operand2 is a register
unsigned int val_reg_last12bits(unsigned int iw, struct emu_analysis_struct *analysis) {
    unsigned int val_in_rm = analysis->regs_read[iw & 0xF]; // regs_read[iw & 0xF] == Rm
    unsigned int sh_op = (iw >>  4) & 0b1;
    unsigned int sh_type = (iw >> 5) & 0b11;
    unsigned int amount;
    if(sh_op == 0){
        amount = (iw >> 7) & 0b11111;
    }else{
        amount = analysis->regs_write[iw >> 8 & 0xF];
    }
    //how_to_shift(sh_type, amount, val_in_rm)
    switch (sh_type) {
        case 0 : val_in_rm << amount;
            break;
        case 1 : val_in_rm >> amount;
            break;
        case 2 : (signed) val_in_rm >> amount;
            break;
        case 3 : (signed) val_in_rm << amount;
            break;
        default :
            exit(1);
    }
    return val_in_rm;
}

void data_processing_instr(struct arm_state *state, struct emu_analysis_struct *analysis){
    unsigned int operand2, immediate, iw, opCode, rd, rn;
    iw = *((unsigned int *) state->regs[PC]);
    immediate = (iw >> 25) & 0b1;
    opCode = (iw >> 21) & 0xF;
    rd = (iw >> 12) & 0xF;
    rn = (iw >> 16) & 0xF;
    if(immediate == 1){
        operand2 = ((iw & 0xFF) << 0xF) || (((iw >> 8) & 0xF) >> 0xFF); // ?????????
    } else{
        operand2 = val_reg_last12bits(iw, analysis);
    }
    switch (opCode) {
        case 0 : state -> regs[rd] = (state -> regs[rn] & operand2) ;
            break;
        case 1 : ;
            break;
        case 2 : state -> regs[rd] = state -> regs[rn] = operand2 ;
            break;
        case 3 : ;
            break;
        case 4 : ;
            break;
        case 8 : ;
            break;
        case 9 : ;
            break;
        case 10 : ;
            break;
        case 12 : ;
            break;
        case 13 : ;
            break;
        default :
            exit(1);
    }
}

void single_data_transfer_instr(struct arm_state *state, struct emu_analysis_struct *analysis){

    unsigned int iw, immediate, offset, p_index, up_bit, load_bit, rn, rd;
    unsigned int data;
    iw = *((unsigned int *) state->regs[PC]);
    immediate = (iw >> 25) & 0b1;//0 - 24 bits 没了(第二十五位
    p_index = (iw >> 24) & 0b1;
    up_bit = (iw >> 23) & 0b1;
    load_bit = (iw >> 20) & 0b1;
    rn = (iw >> 16) & 0xF;
    rd = (iw >> 12) & 0xF;

    //calculate the offset
    if (immediate == 0) {
        offset = iw & 0xFFF;
    } else {
        // Operand2 is a register, return the value of offset / op2
        offset = val_reg_last12bits(iw, analysis);
    }

    if(load_bit == 1) {
        data = *((unsigned int *) state->memory_stack[rn]);
        *((unsigned int *) state -> regs[rn]) = data;
    }

    if(p_index == 1){
        analysis -> regs_read[rn] = state -> regs[rn];
        if(up_bit == 1){
            analysis -> regs_read[rn] += offset;
        } else{
            analysis -> regs_read[rn] -= offset;
        }
        state -> regs[rd] = analysis -> regs_read[rn];
    } else{
        if(up_bit == 1){
            state -> regs[rn] += offset;
        } else{
            state -> regs[rn] -= offset;
        }
        state -> regs[rd] = state -> regs[rn];
    }
}

void multiply_instr(struct arm_state *state, struct emu_analysis_struct *analysis){

}



