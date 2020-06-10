#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "readFile.h"

#define MAX_SIZE_OF_STRING 50

int main(int argc, char **argv) {
  assert(argc == 3);
  int length = getLengthOfFile(argv[1]);
  char **instruction = calloc(length, sizeof(char *));
  if (!instruction) {
    perror("Node allocation failed!");
    exit(EXIT_FAILURE);
  }
  for (int i = 0; i < length; i++) {
    instruction[i] = calloc(MAX_SIZE_OF_STRING, sizeof(char));
    if (!instruction[i]) {
      perror("Node allocation failed!");
      exit(EXIT_FAILURE);
    }
  }
  readSourceFile(argv[1], instruction);
  for (int i = 0; i < length; i++) {
    printf("%s\n", instruction[i]);
  }
  free(instruction);
  for (int i = 0; i < length; i++) {
    free(instruction[i]);
  }
  return EXIT_SUCCESS;
}
