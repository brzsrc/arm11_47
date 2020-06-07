#ifndef INSTRTYPE_H
#define INSTRTYPE_H

void and_instr(decoded *decodedInstr, armstate *state, decoded_dp *decodedDp); //Executes an and instruction

void eor_instr(decoded *decodedInstr, armstate *state, decoded_dp *decodedDp); //Executes an eor instruction

void sub_instr(decoded *decodedInstr, armstate *state, decoded_dp *decodedDp); //Executes a sub instruction

void rsb_instr(decoded *decodedInstr, armstate *state, decoded_dp *decodedDp); //Executes a rsb instruction

void add_instr(decoded *decodedInstr, armstate *state, decoded_dp *decodedDp); //Executes an add instruction

void tst_instr(decoded *decodedInstr, armstate *state, decoded_dp *decodedDp); //Executes a tst instruction

void teq_instr(decoded *decodedInstr, armstate *state, decoded_dp *decodedDp); //Executes a teq instruction

void cmp_instr(decoded *decodedInstr, armstate *state, decoded_dp *decodedDp); //Executes a cmp instruction

void orr_instr(decoded *decodedInstr, armstate *state, decoded_dp *decodedDp); //Executes an orr instruction

void mov_instr(decoded *decodedInstr, armstate *state, decoded_dp *decodedDp); //Executes a mov instruction

void single_data_transfer_instr(decoded *decodedInstr, armstate *state); //Executes a single datat transfer instruction

void multiply_instr(decoded *decodedInstr, armstate *state); //Executes a multiply instruction

void branch_instr(decoded *decodedInstr, armstate *state); //Executes a branch instruction

#endif
