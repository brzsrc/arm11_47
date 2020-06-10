#include <stdio.h>

int getLengthOfFile(char *fileName) {
  FILE *ptr = fopen(fileName, "r");
  int size = 0;

  char c = getc(ptr);
  while (c != EOF) {
    if (c == '\n' || c == ' ') {
      size++;
    }
    c = getc(ptr);
  }
  fclose(ptr);
  return size;
}

void readSourceFile(char *fileName, char **instruction) {
  int i = 0;
  FILE *ptr = fopen(fileName, "r");

  while (!feof(ptr)) {
      fscanf(ptr, "%s",  instruction[i]);
      i++;
  }
  fclose(ptr); //Closes file "filename"
}
