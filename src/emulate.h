#define NREGS 17
#define STACK_SIZE 16384 //Defined stack as ints which are 4 bytes so (64*1024)/4
#define PC 15

void bigToLittleEndian(armstate *state);

void printResult(armstate *state, int numOfInstr);

void startCycle(armstate *state);

int main(int argc, char **argv);
