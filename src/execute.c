#include "execute.h"
#include "macrosAndStructs.h"
#include "instrType.h"
#include <stdlib.h>

void armstate_init(armstate *state) {
    // zero out all arm state
    state->n = 0;
    state->z = 0;
    state->c = 0;
    state->v = 0;
    for (int i = 0; i < NREGS; i++) {
        state->regs[i] = 0;
    }
    state->memory = calloc(STACK_SIZE, sizeof(int));
}

// next_instruction_condition_check
int do_next_instruction(decoded *decodedInstr, armstate *state){
    unsigned int cond = decodedInstr->condition;
    
    if ((cond == COND_EQ) && state->z) return 1;

    if ((cond == COND_NE) && !state->z) return 1;

    if ((cond == COND_GE) && (state->n == state->v)) return 1;

    if ((cond == COND_LT) && (state->n != state->v)) return 1;

    if (cond == COND_GT && !state->z && state->n == state->v) return 1;

    if (cond == COND_LE && (state->z || state->n != state->v)) return 1;

    if (cond == COND_AL) return 1;

    return 0;
}

//Operand2 is a register
void val_reg_last12bits(decoded *decodedInstr, unsigned int *operand2, armstate *state) {
    unsigned int iw = decodedInstr -> bit0to25;
    unsigned int val_in_rm = state -> regs[iw & 0xF]; // state -> regs[iw & 0xF] == Rm
    unsigned int sh_op = (iw >> SHIFT_OP) & 1;
    unsigned int sh_type = (iw >> SHIFT_TYPE) & 3;
    unsigned int amount;
    if(sh_op == 0){
        amount = (iw >> 7) & 31;
    }else{
        amount = state -> regs[iw >> 8 & 0xF];
    }
    //how_to_shift(sh_type, amount, val_in_rm)
    switch (sh_type) {
        case 0 : *operand2 = val_in_rm << amount;
            break;
        case 1 : *operand2 = val_in_rm >> amount;
            break;
        case 2 : *operand2 = (signed) val_in_rm >> amount;
            break;
        case 3 : *operand2 = val_in_rm >> amount || ((val_in_rm & amount) << (32 - amount));
            break;
        default :
            exit(1);
    }
}

// the carry out of shift op
unsigned int shift_carry_out(decoded_dp *decodedDp, decoded *decodedInstr, armstate *state){
    unsigned int immediate, carry_out, val, amount;
    unsigned int sh_type, sh_op;
    unsigned int iw = decodedInstr -> bit0to25;

    immediate = (iw >> SINGLEDATA_I_BIT) & 1;
    if(immediate == 1){
        amount = (iw >> 8) & 0xF;
        val = (iw & 0xFF);
        carry_out = val >> amount & 1;
    }else{
        val = state -> regs[iw & 0xF];
        sh_type = (iw >> 5) & 3;
        sh_op = (iw >>  4) & 1;

        if(sh_op == 0){
            amount = (iw >> 7) & 3;
        }else{
            amount = state -> regs[iw >> 8 & 0xF];
        }

        if(sh_type == 0){
            carry_out = decodedDp -> val_reg_last12bits >> (32 - amount) & 1;
        }else{
            carry_out = decodedDp -> val_reg_last12bits >> amount & 1;
        }
    }
    decodedDp -> carry_out = carry_out;
    return decodedDp -> carry_out;
}

void decode_data_processing(decoded *decodedInstr, decoded_dp *decodedDp, armstate *state){
    unsigned int immediate, iw, operand2;
    iw = decodedInstr -> bit0to25;
    decodedDp -> type = decodedInstr -> type;
    decodedDp -> rd = (iw >> DATA_PROCESS_RD_BIT) & 0xF;
    decodedDp -> rn = (iw >> DATA_PROCESS_RN_BIT) & 0xF;
    decodedDp -> s_bit = (iw >> DATA_PROCESS_S_BIT) & 1;
    immediate = (iw >> DATA_PROCESS_I_BIT) & 1;


    if(immediate == 1){
        int rotate = (iw & 0xf00) >> 7;
        int imm = (iw & 0xff);
        int carry;
        if (rotate != 0) {
        for (int i = 0; i < rotate; i++) {
          carry = imm & 1;
          if (carry == 0) {
            imm = imm >> 1 & 0x7fffffff;
          } else {
            imm = (imm >> 1) | 0x80000000;
          }
        }
        }
        operand2 = imm;
    } else{
        val_reg_last12bits(decodedInstr, &operand2, state);
    }
    decodedDp -> operand2 = operand2;
}

void execute(decoded *decodedInstr, armstate *state){
    if(do_next_instruction(decodedInstr, state)){
    decoded_dp *decodedDp = calloc(1, sizeof(decoded_dp));
        switch(decodedInstr -> type){
            case and:
                and_instr(decodedInstr, state, decodedDp);
                break;
            case eor:
                eor_instr(decodedInstr, state, decodedDp);
                break;
            case sub:
                sub_instr(decodedInstr, state, decodedDp);
                break;
            case rsb:
                rsb_instr(decodedInstr, state, decodedDp);
                break;
            case add:
                add_instr(decodedInstr, state, decodedDp);
                break;
            case tst:
                tst_instr(decodedInstr, state, decodedDp);
                break;
            case teq:
                teq_instr(decodedInstr, state, decodedDp);
                break;
            case cmp:
                cmp_instr(decodedInstr, state, decodedDp);
                break;
            case orr:
                orr_instr(decodedInstr, state, decodedDp);
                break;
            case mov:
                mov_instr(decodedInstr, state, decodedDp);
                break;
            case multiply:
                multiply_instr(decodedInstr, state);
                break;
            case branch:
                branch_instr(decodedInstr, state);
                break;
            case singledata:
                single_data_transfer_instr(decodedInstr, state);
                break;
        }
        free(decodedDp);
    }
        //state->regs[PC] += 4;
}
