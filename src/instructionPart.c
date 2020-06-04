// finished Single Data Transfer instructions,
// did part of the data processing instructions,
// haven't done any test yet

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define NREGS 17
#define STACK_SIZE 65536
#define PC 15

struct arm_state {
    unsigned int regs[NREGS];
    unsigned char memory_stack[STACK_SIZE];
    int n, z, c, v;
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

void arm_state_init(struct arm_state *as, unsigned int *objectCode) {
    /* zero out all arm state */
    state->z = 0;
    state->n = 0;
    state->p = 0;

    for (int i = 0; i < NREGS; i++) {
        as->regs[i] = 0;
    }

    for (int i = 0; i < STACK_SIZE; i++) {
        as->stack[i] = 0;
    }

    as -> regs[PC] = objectCode; //pass the add of the fst elem into regPC
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
        case 3 : val_in_rm >> amount || ((val_in_rm & amount) << (32 - amount)); // ??????????
            break;
        default :
            exit(1);
    }
    return val_in_rm;
}

// the carry out of shift op
unsigned int shift_carry_out(unsigned int iw, struct emu_analysis_struct *analysis){
    unsigned int immediate, carry_out, val, amount;
    unsigned int sh_type, sh_op;

    immediate = (iw >> 25) & 0b1;
    if(immediate == 1){
        amount = (iw >> 8) & 0xF;
        val = (iw & 0xFF);
        carry_out = val >> amount & 0b1;
    }else{
        val = analysis->regs_read[iw & 0xF];
        sh_type = (iw >> 5) & 0b11;
        sh_op = (iw >>  4) & 0b1;

        if(sh_op == 0){
            amount = (iw >> 7) & 0b11111;
        }else{
            amount = analysis->regs_write[iw >> 8 & 0xF];
        }

        if(sh_type == 0){
            carry_out = val_in_rm >> (32 - amount) & 0b1;
        }else{
            carry_out = val_in_rm >> amount & 0b1;
        }
    }
    return carry_out;
}

void and_instr(struct arm_state *state, struct emu_analysis_struct *analysis, unsigned int operand2){
    unsigned int s_bit = (iw >> 20) &0b1;
    unsigned int iw = *((unsigned int *) state->regs[PC]);
    unsigned int res = (state -> regs[rn] & operand2);
    state -> regs[rd] = res;

    if(s_bit == 1){
        state -> c = shift_carry_out(iw, analysis);
        state -> n = (res >> 31) & 0b1;
        if(res == 0){
            state -> z = 1;
        }else{
            state -> z = 0;
        }
    }
}

void eor_instr(struct arm_state *state, struct emu_analysis_struct *analysis, unsigned int operand2){
    unsigned int s_bit = (iw >> 20) &0b1;
    unsigned int iw = *((unsigned int *) state->regs[PC]);
    unsigned res = (state -> regs[rn] ^ operand2);
    state -> regs[rd] = res;

    if(s_bit == 1){
        state -> c = shifte_carry_out(iw, analysis);
        state -> n = (res >> 31) & 0b1;
        if(res == 0){
            state -> z = 1;
        }else{
            state -> z = 0;
        }
    }
}

void sub_instr(struct arm_state *state, struct emu_analysis_struct *analysis, unsigned int operand2){
    unsigned int s_bit = (iw >> 20) &0b1;
    unsigned int iw = *((unsigned int *) state->regs[PC]);
    unsigned res = (state -> regs[rn] - operand2);
    state -> regs[rd] = res;

    if(s_bit == 1){
        if(state -> regs[rn] < operand2){
            state -> c = 0;
        }else{
            state -> c = 1;
        }
        state -> n = (res >> 31) & 0b1;
        if(res == 0){
            state -> z = 1;
        }else{
            state -> z = 0;
        }
    }
}

void rsb_instr(struct arm_state *state, struct emu_analysis_struct *analysis, unsigned int operand2){
    unsigned int s_bit = (iw >> 20) &0b1;
    unsigned int iw = *((unsigned int *) state->regs[PC]);
    unsigned res = operand2 - state -> regs[rn]);
    state -> regs[rd] = res;

    if(s_bit == 1){
        if(operand2 < state -> regs[rn]){
            state -> c = 0;
        }else{
            state -> c = 1;
        }
        state -> n = (res >> 31) & 0b1;
        if(res == 0){
            state -> z = 1;
        }else{
            state -> z = 0;
        }
    }
}

void add_instr(struct arm_state *state, struct emu_analysis_struct *analysis, unsigned int operand2){
    unsigned int s_bit = (iw >> 20) &0b1;
    unsigned int iw = *((unsigned int *) state->regs[PC]);
    unsigned res = operand2 + state -> regs[rn]);
    state -> regs[rd] = res;

    if(s_bit == 1){
        if(res <= 0xFFFFFFFF){
            state -> c = 0;
        }else{
            state -> c = 1;
        }
        state -> n = (res >> 31) & 0b1;
        if(res == 0){
            state -> z = 1;
        }else{
            state -> z = 0;
        }
    }
}

void tst_instr(struct arm_state *state, struct emu_analysis_struct *analysis, unsigned int operand2){
    unsigned int s_bit = (iw >> 20) &0b1;
    unsigned int iw = *((unsigned int *) state->regs[PC]);
    unsigned res = operand2 + state -> regs[rn]);

    if(s_bit == 1){
        if(res <= 0xFFFFFFFF){
            state -> c = 0;
        }else{
            state -> c = 1;
        }
        state -> n = (res >> 31) & 0b1;
        if(res == 0){
            state -> z = 1;
        }else{
            state -> z = 0;
        }
    }
}

void teq_instr(struct arm_state *state, struct emu_analysis_struct *analysis, unsigned int operand2){
    unsigned int s_bit = (iw >> 20) &0b1;
    unsigned int iw = *((unsigned int *) state->regs[PC]);
    unsigned res = (state -> regs[rn] ^ operand2);

    if(s_bit == 1){
        state -> c = shifte_carry_out(iw, analysis);
        state -> n = (res >> 31) & 0b1;
        if(res == 0){
            state -> z = 1;
        }else{
            state -> z = 0;
        }
    }
}

void cmp_instr(struct arm_state *state, struct emu_analysis_struct *analysis, unsigned int operand2){
    unsigned int s_bit = (iw >> 20) &0b1;
    unsigned int iw = *((unsigned int *) state->regs[PC]);
    unsigned res = (state -> regs[rn] - operand2);

    if(s_bit == 1){
        if(state -> regs[rn] < operand2){
            state -> c = 0;
        }else{
            state -> c = 1;
        }
        state -> n = (res >> 31) & 0b1;
        if(res == 0){
            state -> z = 1;
        }else{
            state -> z = 0;
        }
    }
}

void orr_instr(struct arm_state *state, struct emu_analysis_struct *analysis, unsigned int operand2){
    unsigned int s_bit = (iw >> 20) &0b1;
    unsigned int iw = *((unsigned int *) state->regs[PC]);
    unsigned int res = (state -> regs[rn] || operand2);
    state -> regs[rd] = res;

    if(s_bit == 1){
        state -> c = shift_carry_out(iw, analysis);
        state -> n = (res >> 31) & 0b1;
        if(res == 0){
            state -> z = 1;
        }else{
            state -> z = 0;
        }
    }
}

void mov_instr(struct arm_state *state, struct emu_analysis_struct *analysis, unsigned int operand2){
    unsigned int s_bit = (iw >> 20) &0b1;
    unsigned int iw = *((unsigned int *) state->regs[PC]);
    state -> regs[rd] = operand2;

    if(s_bit == 1){
        state -> c = shift_carry_out(iw, analysis);
        state -> n = (res >> 31) & 0b1;
        if(res == 0){
            state -> z = 1;
        }else{
            state -> z = 0;
        }
    }
}

void data_processing_instr(struct arm_state *state, struct emu_analysis_struct *analysis){
    unsigned int operand2, immediate, iw, opCode, rd, rn, s_bit, amount;
    iw = *((unsigned int *) state->regs[PC]);
    immediate = (iw >> 25) & 0b1;
    opCode = (iw >> 21) & 0xF;
    rd = (iw >> 12) & 0xF;
    rn = (iw >> 16) & 0xF;

    if(immediate == 1){
        amount = (iw >> 8) & 0xF;
        operand2 = (iw & 0xFF); // 32-bit
        operand2 = operand2 >> (amount) || (operand2 & amount) << (32 - amount);
    } else{
        operand2 = val_reg_last12bits(iw, analysis);
    }
    switch (opCode) {
        case 0 : and_instr(state, analysis, operand2);
            break;
        case 1 : eor_instr(state, analysis, operand2);
            break;
        case 2 : sub_instr(state, analysis, operand2);
            break;
        case 3 : rsb_instr(state, analysis, operand2);
            break;
        case 4 : add_instr(state, analysis, operand2);
            break;
        case 8 : tst_instr(state, analysis, operand2);
            break;
        case 9 : teq_instr(state, analysis, operand2);
            break;
        case 10 : cmp_instr(state, analysis, operand2);
            break;
        case 12 : orr_instr(state, analysis, operand2);
            break;
        case 13 : mov_instr(state, analysis, operand2);
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
            analysis -> regs_read[rn] += offset; // ?????????????????
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
    unsigned iw, acc, set, rd, rn, rs, rm, res;
    iw = *((unsigned int *) state->regs[PC]);
    acc = (iw >> 21) & 0b1;
    set = (iw >> 20) & 0b1;
    rd = (iw >> 16) & 0xF;
    rn = (iw >> 12) & 0xF;
    rs = (iw >> 8) & 0xF;
    rm = iw & 0xF;

    if(acc == 1){
        state -> regs[rd] = state -> regs[rm] * state -> regs[rs] + state -> regs[rn];
    }else{
        state -> regs[rd] = state -> regs[rm] * state -> regs[rs];
    }
    res = state -> regs[rd];

    if(set == 1){
        state -> n = (res >> 31) & 0b1;
        if(res == 0){
            state -> z = 1;
        }else{
            state -> z = 0;
        }
    }

}

//not sure about branch execution///////////////
void branch_instr(struct arm_state *state, struct emu_analysis_struct *analysis){
    signed int offset, type;
    unsigned int iw = *((unsigned int *) state->regs[PC]);

    // shifted left 2 bits, sign extended to 32 bits??????????????
    offset = ((signed) (iw & 0xFFFFFF)) << 2;
    state -> regs[PC] += offset;
}

//from sam's codes
int *readFile(char *filename) {
    int *objectCode = calloc(16384, sizeof(int)); //Creates a pointer with allocated space of 64KB
    int i = 0;
    FILE *ptr = fopen(filename, "rb"); //Opens file "filename" to be read as a binary file
    assert(ptr != NULL); //Checks the file has been read

    while (!feof(ptr)) {
        fread(objectCode + i, 4, 1, ptr); //Stores the binary data in the files into the array objectCode
        i++;
    }

    fclose(ptr); //Closes file "filename"

    return objectCode; // each elem in objectCode is an instr
}

