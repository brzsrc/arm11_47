#ifndef EMULATE_H
#define EMULATE_H

void bigToLittleEndian(armstate *state);

void printResult(armstate *state, int numOfInstr);

void startCycle(armstate *state);

int main(int argc, char **argv);
#endif
