#include "macrosAndStructs.h"
#include "execute.h"
#include <stdlib.h>
#include <stdio.h>

void setZFlag(unsigned int res, armstate *state) {
  if (!res) {
    state->z = 1;
  } else {
    state->z = 0;
  }
}

void and_instr(decoded *decodedInstr, armstate *state, decoded_dp *decodedDp){

    decode_data_processing(decodedInstr, decodedDp, state);
    unsigned int res = (state -> regs[decodedDp->rn] & decodedDp->operand2);
    state -> regs[decodedDp -> rd] = res;

    if(decodedDp->s_bit == 1){
        state->c = shift_carry_out(decodedDp, decodedInstr, state);
        state->n = (res >> 31) & 1;
        setZFlag(res, state);
    }
}

void eor_instr(decoded *decodedInstr, armstate *state, decoded_dp *decodedDp){

    decode_data_processing(decodedInstr, decodedDp, state);
    unsigned int res = (state->regs[decodedDp->rn] ^ decodedDp->operand2);
    state->regs[decodedDp->rd] = res;

    if(decodedDp->s_bit == 1){
        state->c = shift_carry_out(decodedDp, decodedInstr, state);
        state->n = (res >> 31) & 1;
        setZFlag(res, state);
    }
}

void sub_instr(decoded *decodedInstr, armstate *state, decoded_dp *decodedDp){
    decode_data_processing(decodedInstr, decodedDp, state);
    unsigned int res = (state -> regs[decodedDp -> rn] - decodedDp -> operand2);
    state -> regs[decodedDp -> rd] = res;

    if(decodedDp->s_bit == 1){
        if(state->regs[decodedDp->rn] < decodedDp->operand2){
            state->c = 0;
        }else{
            state->c = 1;
        }
        state->n = (res >> 31) & 1;
        setZFlag(res, state);
    }
}

void rsb_instr(decoded *decodedInstr, armstate *state, decoded_dp *decodedDp){

    decode_data_processing(decodedInstr, decodedDp, state);
    unsigned int res = decodedDp -> operand2 - state -> regs[decodedDp -> rn];
    state -> regs[decodedDp -> rd] = res;

    if(decodedDp->s_bit == 1){
        if(decodedDp->operand2 < state -> regs[decodedDp -> rn]){
            state->c = 0;
        }else{
            state -> c = 1;
        }
        state -> n = (res >> 31) & 1;
        setZFlag(res, state);
    }
}

void add_instr(decoded *decodedInstr, armstate *state, decoded_dp *decodedDp){

    decode_data_processing(decodedInstr, decodedDp, state);
    unsigned int res = decodedDp -> operand2 + state -> regs[decodedDp -> rn];
    state -> regs[decodedDp -> rd] = res;

    if(decodedDp -> s_bit == 1){
        if(res <= 0xFFFFFFFF){
            state -> c = 0;
        }else{
            state -> c = 1;
        }
        state -> n = (res >> 31) & 1;
        setZFlag(res, state);
    }
}

void tst_instr(decoded *decodedInstr, armstate *state, decoded_dp *decodedDp){

    decode_data_processing(decodedInstr, decodedDp, state);
    unsigned int res = decodedDp -> operand2 & state -> regs[decodedDp -> rn];

    if(decodedDp -> s_bit == 1){
        if(res <= 0xFFFFFFFF){
            state -> c = 0;
        }else{
            state -> c = 1;
        }
        state -> n = (res >> 31) & 1;
        setZFlag(res, state);
    }
}

void teq_instr(decoded *decodedInstr, armstate *state, decoded_dp *decodedDp){

    decode_data_processing(decodedInstr, decodedDp, state);
    unsigned res = (state -> regs[decodedDp -> rn] ^ decodedDp -> operand2);

    if(decodedDp -> s_bit == 1){
        state -> c = shift_carry_out(decodedDp, decodedInstr, state);
        state -> n = (res >> 31) & 1;
        setZFlag(res, state);
    }
}

void cmp_instr(decoded *decodedInstr, armstate *state, decoded_dp *decodedDp){
    decode_data_processing(decodedInstr, decodedDp, state);
    unsigned res = (state->regs[decodedDp->rn] - decodedDp->operand2);

    if(decodedDp->s_bit == 1){
        if(state -> regs[decodedDp -> rn] < decodedDp -> operand2){
            state -> c = 0;
        }else{
            state -> c = 1;
        }
        state -> n = (res >> 31) & 1;
        setZFlag(res, state);
    }
}

void orr_instr(decoded *decodedInstr, armstate *state, decoded_dp *decodedDp){

    decode_data_processing(decodedInstr, decodedDp, state);
    unsigned int res = (state -> regs[decodedDp -> rn] | decodedDp -> operand2);
    state -> regs[decodedDp -> rd] = res;

    if(decodedDp -> s_bit == 1){
        state -> c = shift_carry_out(decodedDp, decodedInstr, state);
        state -> n = (res >> 31) & 1;
        setZFlag(res, state);
    }
}

void mov_instr(decoded *decodedInstr, armstate *state, decoded_dp *decodedDp){
    decode_data_processing(decodedInstr, decodedDp, state);
    state -> regs[decodedDp -> rd] = decodedDp -> operand2;

    if(decodedDp -> s_bit == 1){
        state -> c = shift_carry_out(decodedDp, decodedInstr, state);
        state -> n = (decodedDp -> operand2 >> 31) & 1;
        setZFlag(decodedDp->operand2, state);
    }
}


void single_data_transfer_instr(decoded *decodedInstr, armstate *state){

    unsigned int iw, immediate, offset, p_index, up_bit, load_bit, rn, rd;
    unsigned int data;

    iw = decodedInstr->bit0to25;
    immediate = (iw >> SINGLEDATA_I_BIT) & 1;//0 - 24 bits 没了(第二十五位
    p_index = (iw >> SINGLEDATA_P_BIT) & 1;
    up_bit = (iw >> SINGLEDATA_UP_BIT) & 1;
    load_bit = (iw >> SINGLEDATA_LOAD_BIT) & 1;
    rn = (iw >> SINGLEDATA_RN_BIT) & 0xF;
    rd = (iw >> SINGLEDATA_RD_BIT) & 0xF;

    //calculate the offset
    if (immediate == 0) {
        offset = iw & 0xFFF;
    } else {
        // Operand2 is a register, return the value of offset / op2
        val_reg_last12bits(decodedInstr, &offset, state);
    }

    if(p_index == 1){
        if(up_bit == 1){
            data = offset + state->regs[rn];
        } else{
            data = state->regs[rn] - offset;
        }
    } else{
        if(up_bit == 1){
        	data = state->regs[rn];
            state->regs[rn] += offset;
        } else{
        	data = state->regs[rn];
            state->regs[rn] -= offset;
        }
    }
    char *byteMem = (char *) state->memory;
    int *memAdd = (int *)(byteMem + data);

    if(load_bit == 1) {
    	if (data < MEMORY_SIZE_BYTES) {
          state->regs[rd] = *memAdd;
        } else {
          printf("Error: Out of bounds memory access at address 0x%08x\n", data);
        }
    } else {
    	if ((data < MEMORY_SIZE_BYTES && immediate == 0) || immediate == 1) {
          *memAdd = state->regs[rd];
        } else {
          printf("Error: Out of bounds memory access at address 0x%08x\n", data);
        }
    }
}

void multiply_instr(decoded *decodedInstr, armstate *state){
    unsigned iw, acc, set, rd, rn, rs, rm, res;
    iw = decodedInstr -> bit0to25;;
    acc = (iw >> MUL_ACC_BIT) & 1;
    set = (iw >> MUL_S_BIT) & 1;
    rd = (iw >> MUL_RD_BIT) & 0xf;
    rn = (iw >> MUL_RN_BIT) & 0xf;
    rs = (iw >> MUL_RS_BIT) & 0xf;
    rm = iw & 0xf;

    if(acc == 1){
        state->regs[rd] = state->regs[rm] * state->regs[rs] + state->regs[rn];
    }else{
        state->regs[rd] = state->regs[rm] * state->regs[rs];
    }
    res = state->regs[rd];

    if(set == 1){
        state->n = (res >> 31) & 1;
        setZFlag(res, state);
    }

}

void branch_instr(decoded *decodedInstr, armstate *state){

    signed int offset;
    unsigned int iw = decodedInstr->bit0to25;

    offset = (iw & 0x00ffffff) << 2;
    int sign = (iw >> 23) & 1;
    if (sign == 1) {
      offset = offset | 0xff000000;
    }
    state->regs[PC] += offset;
}
