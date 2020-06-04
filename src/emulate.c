#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define NREGS 17
#define STACK_SIZE 16384 //Defined stack as ints which are 4 bytes so (64*1024)/4
#define PC 15

typedef enum {and, eor, sub, rsb, add, tst, teq, cmp, orr, mov, multiply, branch, singledata}type;

typedef struct decoded {
    type type;
    unsigned int condition;
    unsigned int bit0to25;
} decoded;

typedef struct decoded_data_processing {
    type type;
    unsigned int rn, rd, operand2, s_bit;
    unsigned int carry_out, val_reg_last12bits;
} decoded_dp;

typedef struct arm_state {
    unsigned int regs[NREGS];
    unsigned int *memory; // *objectcode
    int n, z, c, v;
} armstate;

void armstate_init(armstate *state, unsigned int *objectCode) {
    /* zero out all arm state */
    state->n = 0;
    state->z = 0;
    state->c = 0;
    state->v = 0;

    for (int i = 0; i < NREGS; i++) {
        state->regs[i] = 0;
    }

    state -> memory = objectCode;
}

char *printenum(type t) { //Testing
    switch(t) {
        case and:
            return "and";
            break;
        case eor:
            return "eor";
            break;
        case sub:
            return "sub";
            break;
        case rsb:
            return "rsb";
            break;
        case add:
            return "add";
            break;
        case tst:
            return "tst";
            break;
        case teq:
            return "cmp";
            break;
        case cmp:
            return "cmp";
            break;
        case orr:
            return "orr";
            break;
        case mov:
            return "mov";
            break;
        case multiply:
            return "mul";
            break;
        case branch:
            return "branch";
            break;
        case singledata:
            return "singledata";
            break;
    }
    return "NULL";
}

void findOpcode(decoded *decodedInstr, unsigned int instruction) {
    unsigned int opcode = (instruction >> 21) & 0xf;

    if (opcode == 0) {
        decodedInstr->type = and;
    } else if (opcode == 1) {
        decodedInstr->type = eor;
    } else if (opcode == 2) {
        decodedInstr->type = sub;
    } else if (opcode == 3) {
        decodedInstr->type = rsb;
    } else if (opcode == 4) {
        decodedInstr->type = add;
    } else if (opcode == 8) {
        decodedInstr->type = tst;
    } else if (opcode == 9) {
        decodedInstr->type = teq;
    } else if (opcode == 10) {
        decodedInstr->type = cmp;
    } else if (opcode == 12) {
        decodedInstr->type = orr;
    } else if (opcode == 13) {
        decodedInstr->type = mov;
    }
}

// next_instruction_condition_check
int do_next_instruction(decoded *decodedInstr, armstate *state){
    unsigned int cond = decodedInstr -> condition;
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
void val_reg_last12bits(decoded *decodedInstr, unsigned int *operand2, armstate *state) {
    unsigned int iw = decodedInstr -> bit0to25;
    unsigned int val_in_rm = state -> regs[iw & 0xF]; // state -> regs[iw & 0xF] == Rm
    unsigned int sh_op = (iw >>  4) & 0b1;
    unsigned int sh_type = (iw >> 5) & 0b11;
    unsigned int amount;
    if(sh_op == 0){
        amount = (iw >> 7) & 0b11111;
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
        case 3 : *operand2 = val_in_rm >> amount || ((val_in_rm & amount) << (32 - amount)); // ??????????
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

    immediate = (iw >> 25) & 0b1;
    if(immediate == 1){
        amount = (iw >> 8) & 0xF;
        val = (iw & 0xFF);
        carry_out = val >> amount & 0b1;
    }else{
        val = state -> regs[iw & 0xF];
        sh_type = (iw >> 5) & 0b11;
        sh_op = (iw >>  4) & 0b1;

        if(sh_op == 0){
            amount = (iw >> 7) & 0b11111;
        }else{
            amount = state -> regs[iw >> 8 & 0xF];
        }

        if(sh_type == 0){
            carry_out = decodedDp -> val_reg_last12bits >> (32 - amount) & 0b1;
        }else{
            carry_out = decodedDp -> val_reg_last12bits >> amount & 0b1;
        }
    }
    decodedDp -> carry_out = carry_out;
    return decodedDp -> carry_out;
}

void decode_data_processing(decoded *decodedInstr, decoded_dp *decodedDp, armstate *state){
    unsigned int immediate, iw, operand2;
    iw = decodedInstr -> bit0to25;
    decodedDp -> type = decodedInstr -> type;
    decodedDp -> rd = (iw >> 12) & 0xF;
    decodedDp -> rn = (iw >> 16) & 0xF;
    decodedDp -> s_bit = (iw >> 20) & 0b1;
    immediate = (iw >> 25) & 0b1;


    if(immediate == 1){
        operand2 = iw & 0xfff;
    } else{
        val_reg_last12bits(decodedInstr, &operand2, state);
    }
    decodedDp -> operand2 = operand2; 
    printf("operand2: %d\n", operand2);
}


void and_instr(decoded *decodedInstr, armstate *state){
    decoded_dp *decodedDp = calloc(1, sizeof(decoded_dp));

    decode_data_processing(decodedInstr, decodedDp, state);
    unsigned int res = (state -> regs[decodedDp -> rn] & decodedDp -> operand2);
    state -> regs[decodedDp -> rd] = res;

    if(decodedDp -> s_bit == 1){
        state -> c = shift_carry_out(decodedDp, decodedInstr, state);
        state -> n = (res >> 31) & 0b1;
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
        state -> n = (res >> 31) & 0b1;
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
        state -> n = (res >> 31) & 0b1;
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
        state -> n = (res >> 31) & 0b1;
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
        state -> n = (res >> 31) & 0b1;
        if(res == 0){
            state -> z = 1;
        }else{
            state -> z = 0;
        }
    }
}

void tst_instr(decoded *decodedInstr, armstate *state){
    decoded_dp *decodedDp = calloc(1, sizeof(decoded_dp));

    decode_data_processing(decodedInstr, decodedDp, state);
    unsigned int res = decodedDp -> operand2 + state -> regs[decodedDp -> rn];

    if(decodedDp -> s_bit == 1){
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

void teq_instr(decoded *decodedInstr, armstate *state){
    decoded_dp *decodedDp = calloc(1, sizeof(decoded_dp));

    decode_data_processing(decodedInstr, decodedDp, state);
    unsigned res = (state -> regs[decodedDp -> rn] ^ decodedDp -> operand2);

    if(decodedDp -> s_bit == 1){
        state -> c = shift_carry_out(decodedDp, decodedInstr, state);
        state -> n = (res >> 31) & 0b1;
        if(res == 0){
            state -> z = 1;
        }else{
            state -> z = 0;
        }
    }
}

void cmp_instr(decoded *decodedInstr, armstate *state){
    decoded_dp *decodedDp = calloc(1, sizeof(decoded_dp));

    decode_data_processing(decodedInstr, decodedDp, state);
    unsigned res = (state -> regs[decodedDp -> rn] - decodedDp -> operand2);

    if(decodedDp -> s_bit == 1){
        if(state -> regs[decodedDp -> rn] < decodedDp -> operand2){
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

void orr_instr(decoded *decodedInstr, armstate *state){
    decoded_dp *decodedDp = calloc(1, sizeof(decoded_dp));

    decode_data_processing(decodedInstr, decodedDp, state);
    unsigned int res = (state -> regs[decodedDp -> rn] || decodedDp -> operand2);
    state -> regs[decodedDp -> rd] = res;

    if(decodedDp -> s_bit == 1){
        state -> c = shift_carry_out(decodedDp, decodedInstr, state);
        state -> n = (res >> 31) & 0b1;
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
        state -> n = (decodedDp -> operand2 >> 31) & 0b1;
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
    iw = decodedInstr -> bit0to25;
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
        val_reg_last12bits(decodedInstr, &offset, state);
    }

    if(load_bit == 1) {
        data = state->memory[rn];
        state->regs[rn] = data;
    }

    if(p_index == 1){
        if(up_bit == 1){
            state -> regs[rn] += offset;
        } else{
            state -> regs[rn] -= offset;
        }
        state -> regs[rd] = state -> regs[rn];
    } else{
        state -> regs[rd] = state -> regs[rn];
        if(up_bit == 1){
            state -> regs[rn] += offset;
        } else{
            state -> regs[rn] -= offset;
        }
    }
}

void multiply_instr(decoded *decodedInstr, armstate *state){
    unsigned iw, acc, set, rd, rn, rs, rm, res;
    iw = decodedInstr -> bit0to25;;
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
void branch_instr(decoded *decodedInstr, armstate *state){
    signed int offset;
    unsigned int iw = decodedInstr -> bit0to25;

    // shifted left 2 bits, sign extended to 32 bits??????????????
    offset = ((signed) (iw & 0xFFFFFF)) << 2;
    state -> regs[PC] += offset;
}


unsigned int fetch(unsigned int *objectcode, unsigned int programCounter) {
    return objectcode[programCounter]; //Returns an instruction at the value of programCounter
}

void decode(unsigned int instruction, decoded *decodedInstr) {
    unsigned int bit26and27 = (instruction >> 26) & 0b11;

    decodedInstr->condition = (instruction >> 28) & 0xf;
    decodedInstr->bit0to25 = (instruction) & 0x3ffffff;

    if (bit26and27 == 0) {
        findOpcode(decodedInstr, instruction);
    } else if (bit26and27 == 1) {
        decodedInstr->type = singledata;
    } else if (bit26and27 == 2) {
        decodedInstr->type = branch;
    }
}

void execute(decoded *decodedInstr, armstate *state){
    if(do_next_instruction(decodedInstr, state)){
        switch(decodedInstr -> type){
            case and:
                and_instr(decodedInstr, state);
                break;
            case eor:
                eor_instr(decodedInstr, state);
                break;
            case sub:
                sub_instr(decodedInstr, state);
                break;
            case rsb:
                rsb_instr(decodedInstr, state);
                break;
            case add:
                add_instr(decodedInstr, state);
                break;
            case tst:
                tst_instr(decodedInstr, state);
                break;
            case teq:
                teq_instr(decodedInstr, state);
                break;
            case cmp:
                cmp_instr(decodedInstr, state);
                break;
            case orr:
                orr_instr(decodedInstr, state);
                break;
            case mov:
                mov_instr(decodedInstr, state);
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
    }else
        state->regs[PC] += 4;
}

void startCycle(unsigned int *objectcode, armstate *state) {
    unsigned int *pc = &(state->regs[PC]); //pc takes the value of the program counter
    unsigned int fetched;
    decoded *decodedInstr = (decoded *) calloc(1, sizeof(decoded));
    bool finished = false;

  while(!finished) {
      if (*pc == 0) {
          fetched = fetch(objectcode, *pc/4);
          *pc = *pc + 4;
      } else if (*pc == 4) {
          decode(fetched, decodedInstr);
          fetched = fetch(objectcode, *pc/4);
          *pc = *pc + 4;
      } else {
        execute(decodedInstr, state);
        decode(fetched, decodedInstr);
        fetched = fetch(objectcode, *pc/4);
        *pc = *pc + 4;
        if (decodedInstr->condition == 0 && decodedInstr->bit0to25 == 0 && decodedInstr->type == and){
          finished = true;
        }
      }
      
        printf("fetched instruction: %d\n", fetched);
        printf("decoded type: %s\n", printenum(decodedInstr->type)); //Used for testing
        for(int i = 0; i < NREGS; i++){
            printf("register[%d]: %d\n", i, state -> regs[i]);
        }
    }

    free(decodedInstr);
} //pipeline

unsigned int *readFile(char *filename) {
    unsigned int *objectCode = calloc(16384, sizeof(int));
    //Creates a pointer with allocated space of 64KB
    int i = 0;
    FILE *ptr = fopen(filename, "rb"); //Opens file "filename" to be read as a binary file
    assert(ptr != NULL); //Checks the file has been read

    while (!feof(ptr)) {
        fread(objectCode + i, 4, 1, ptr);
        //Stores the binary data in the files into the array objectCode
        i++;
    }
    fclose(ptr); //Closes file "filename"
    return objectCode;
} //loader

int main(int argc, char **argv) {

    unsigned int *objectcode = calloc(STACK_SIZE, sizeof(int));
    //unsigned int registers[NREGS];
    armstate *state = (armstate *) calloc(1, sizeof(armstate));

    armstate_init(state, objectcode); // &??????????

    assert(argc == 2);
    objectcode = readFile(argv[1]);

    startCycle(objectcode, state); //???????

    free(objectcode);
    free(state);
    return EXIT_SUCCESS;
}

