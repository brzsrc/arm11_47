#include "execute.h"
#include <stdlib.h>

void and_instr(decoded *decodedInstr, armstate *state){
    decoded_dp *decodedDp = calloc(1, sizeof(decoded_dp));

    decode_data_processing(decodedInstr, decodedDp, state);
    unsigned int res = (state -> regs[decodedDp -> rn] & decodedDp -> operand2);
    state -> regs[decodedDp -> rd] = res;

    if(decodedDp -> s_bit == 1){
        state -> c = shift_carry_out(decodedDp, decodedInstr, state);
        state -> n = (res >> 31) & 1;
        if(res == 0){
            state -> z = 1;
        }else{
            state -> z = 0;
        }
    }
    free(decodedDp);
}

void eor_instr(decoded *decodedInstr, armstate *state){
    decoded_dp *decodedDp = calloc(1, sizeof(decoded_dp));

    decode_data_processing(decodedInstr, decodedDp, state);
    unsigned int res = (state -> regs[decodedDp -> rn] ^ decodedDp -> operand2);
    state -> regs[decodedDp -> rd] = res;

    if(decodedDp -> s_bit == 1){
        state -> c = shift_carry_out(decodedDp, decodedInstr, state);
        state -> n = (res >> 31) & 1;
        if(res == 0){
            state -> z = 1;
        }else{
            state -> z = 0;
        }
    }
    free(decodedDp);
}

void sub_instr(decoded *decodedInstr, armstate *state){
    decoded_dp *decodedDp = calloc(1, sizeof(decoded_dp));

    decode_data_processing(decodedInstr, decodedDp, state);
    unsigned int res = (state -> regs[decodedDp -> rn] - decodedDp -> operand2);
    state -> regs[decodedDp -> rd] = res;

    if(decodedDp -> s_bit == 1){
        if(state -> regs[decodedDp -> rn] < decodedDp -> operand2){
            state -> c = 0;
        }else{
            state -> c = 1;
        }
        state -> n = (res >> 31) & 1;
        if(res == 0){
            state -> z = 1;
        }else{
            state -> z = 0;
        }
    }
    free(decodedDp);
}

void rsb_instr(decoded *decodedInstr, armstate *state){
    decoded_dp *decodedDp = calloc(1, sizeof(decoded_dp));

    decode_data_processing(decodedInstr, decodedDp, state);
    unsigned int res = decodedDp -> operand2 - state -> regs[decodedDp -> rn];
    state -> regs[decodedDp -> rd] = res;

    if(decodedDp -> s_bit == 1){
        if(decodedDp -> operand2 < state -> regs[decodedDp -> rn]){
            state -> c = 0;
        }else{
            state -> c = 1;
        }
        state -> n = (res >> 31) & 1;
        if(res == 0){
            state -> z = 1;
        }else{
            state -> z = 0;
        }
    }
    free(decodedDp);
}

void add_instr(decoded *decodedInstr, armstate *state){
    decoded_dp *decodedDp = calloc(1, sizeof(decoded_dp));

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
        if(res == 0){
            state -> z = 1;
        }else{
            state -> z = 0;
        }
    }
    free(decodedDp);
}

void tst_instr(decoded *decodedInstr, armstate *state){
    decoded_dp *decodedDp = calloc(1, sizeof(decoded_dp));

    decode_data_processing(decodedInstr, decodedDp, state);
    unsigned int res = decodedDp -> operand2 & state -> regs[decodedDp -> rn];

    if(decodedDp -> s_bit == 1){
        if(res <= 0xFFFFFFFF){
            state -> c = 0;
        }else{
            state -> c = 1;
        }
        state -> n = (res >> 31) & 1;
        if(res == 0){
            state -> z = 1;
        }else{
            state -> z = 0;
        }
    }
    free(decodedDp);
}

void teq_instr(decoded *decodedInstr, armstate *state){
    decoded_dp *decodedDp = calloc(1, sizeof(decoded_dp));

    decode_data_processing(decodedInstr, decodedDp, state);
    unsigned res = (state -> regs[decodedDp -> rn] ^ decodedDp -> operand2);

    if(decodedDp -> s_bit == 1){
        state -> c = shift_carry_out(decodedDp, decodedInstr, state);
        state -> n = (res >> 31) & 1;
        if(res == 0){
            state -> z = 1;
        }else{
            state -> z = 0;
        }
    }
    free(decodedDp);
}

void cmp_instr(decoded *decodedInstr, armstate *state){
    decoded_dp *decodedDp = calloc(1, sizeof(decoded_dp));

    decode_data_processing(decodedInstr, decodedDp, state);
    unsigned res = (state->regs[decodedDp->rn] - decodedDp->operand2);

    if(decodedDp->s_bit == 1){
        if(state -> regs[decodedDp -> rn] < decodedDp -> operand2){
            state -> c = 0;
        }else{
            state -> c = 1;
        }
        state -> n = (res >> 31) & 1;
        if(res == 0){
            state -> z = 1;
        }else{
            state -> z = 0;
        }
    }
    free(decodedDp);
}

void orr_instr(decoded *decodedInstr, armstate *state){
    decoded_dp *decodedDp = calloc(1, sizeof(decoded_dp));

    decode_data_processing(decodedInstr, decodedDp, state);
    unsigned int res = (state -> regs[decodedDp -> rn] | decodedDp -> operand2);
    state -> regs[decodedDp -> rd] = res;

    if(decodedDp -> s_bit == 1){
        state -> c = shift_carry_out(decodedDp, decodedInstr, state);
        state -> n = (res >> 31) & 1;
        if(res == 0){
            state -> z = 1;
        }else{
            state -> z = 0;
        }
    }
    free(decodedDp);
}

void mov_instr(decoded *decodedInstr, armstate *state){
    decoded_dp *decodedDp = calloc(1, sizeof(decoded_dp));

    decode_data_processing(decodedInstr, decodedDp, state);
    state -> regs[decodedDp -> rd] = decodedDp -> operand2;

    if(decodedDp -> s_bit == 1){
        state -> c = shift_carry_out(decodedDp, decodedInstr, state);
        state -> n = (decodedDp -> operand2 >> 31) & 1;
        if(decodedDp -> operand2 == 0){
            state -> z = 1;
        }else{
            state -> z = 0;
        }
    }
    free(decodedDp);
}


void single_data_transfer_instr(decoded *decodedInstr, armstate *state){

    unsigned int iw, immediate, offset, p_index, up_bit, load_bit, rn, rd;
    unsigned int data;
    
    iw = decodedInstr->bit0to25;
    immediate = (iw >> 25) & 1;//0 - 24 bits 没了(第二十五位
    p_index = (iw >> 24) & 1;
    up_bit = (iw >> 23) & 1;
    load_bit = (iw >> 20) & 1;
    rn = (iw >> 16) & 0xF;
    rd = (iw >> 12) & 0xF;

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
    
    if(load_bit == 1) {
    	char *byteMem = (char *) state->memory;
    	int *memAdd = (int *)(byteMem + data);
        state->regs[rd] = *memAdd;
    } else {
        /*char *byteMem = (char*) state->memory;
    	char *memAdd;
    	memAdd = byteMem + data;
        *memAdd = state->regs[rn];*/
    }
}

void multiply_instr(decoded *decodedInstr, armstate *state){
    unsigned iw, acc, set, rd, rn, rs, rm, res;
    iw = decodedInstr -> bit0to25;;
    acc = (iw >> 21) & 1;
    set = (iw >> 20) & 1;
    rd = (iw >> 16) & 0xf;
    rn = (iw >> 12) & 0xf;
    rs = (iw >> 8) & 0xf;
    rm = iw & 0xf;

    if(acc == 1){
        state -> regs[rd] = state -> regs[rm] * state -> regs[rs] + state -> regs[rn];
    }else{
        state -> regs[rd] = state -> regs[rm] * state -> regs[rs];
    }
    res = state -> regs[rd];

    if(set == 1){
        state -> n = (res >> 31) & 1;
        if(res == 0){
            state -> z = 1;
        }else{
            state -> z = 0;
        }
    }

}

//not sure about branch execution///////////////
void branch_instr(decoded *decodedInstr, armstate *state){
    signed int offset;
    unsigned int iw = decodedInstr->bit0to25;

    // shifted left 2 bits, sign extended to 32 bits??????????????
    offset = (iw & 0x00ffffff) << 2;
    int sign = (iw >> 23) & 1;
    if (sign == 1) {
      offset = offset | 0xff000000;
    }
    state->regs[PC] += offset;
}
