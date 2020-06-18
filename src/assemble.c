#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "assemble_utils.h"

#define MAX_SIZE_OF_STRING 50

int main(int argc, char **argv) {
  if (argc != 3) {
    perror("Must only provide two arguments!");
    exit(EXIT_FAILURE);
  }
  FILE *fileSrc = fopen(argv[1], "r"); //reads file
  FILE *fileDest = fopen(argv[2], "w"); //writes file

  second_pass(fileSrc, fileDest);

  fclose(fileSrc);
  fclose(fileDest);
  return EXIT_SUCCESS;
}
