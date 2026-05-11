#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"

void handle_mov_reg_rm(FILE *f, char *instr) {
  char mod[3], reg[4], rm[4];

  int c = fgetc(f);

  for (int i = 0; i <= 7; ++i) {
    int b = c & (1 << i);

    if (i >= 0 && i < 3) {
      rm[abs(i - 2)] = binchar(b);
      continue;
    }

    if (i >= 3 && i < 6) {
      reg[abs(i - 5)] = binchar(b);
      continue;
    }

    if (i > 5) {
      mod[abs(i - 7)] = binchar(b);
      continue;
    }
  }

  mod[2] = '\0';
  reg[3] = '\0';
  rm[3] = '\0';

  if (strcmp(mod, "00") == 0) {
    printf("mov ");

    if (instr[6] == '1') {
      printf("%s, [%s]\n", get_reg(reg, instr[7]), get_rm(rm));
    } else {
      printf("[%s], %s\n", get_rm(rm), get_reg(reg, instr[7]));
    }

    return;
  }

  if (strcmp(mod, "01") == 0) {
    printf("mov ");

    int b = fgetc(f);
    unsigned int disp8 = b & 0xFF;

    if (instr[6] == '1') {
      if (disp8)
        printf("%s, [%s + %d]\n", get_reg(reg, instr[7]), get_rm(rm), disp8);
      else
        printf("%s, [%s]\n", get_reg(reg, instr[7]), get_rm(rm));
    } else {
      if (disp8)
        printf("[%s + %d], %s\n", get_rm(rm), disp8, get_reg(reg, instr[7]));
      else
        printf("[%s], %s\n", get_rm(rm), get_reg(reg, instr[7]));
    }

    return;
  }

  if (strcmp(mod, "10") == 0) {
    printf("mov ");

    int b1 = fgetc(f);
    int b2 = fgetc(f);
    unsigned int disp8l = b1 & 0xFF;
    unsigned int disp8h = b2 & 0xFF;
    unsigned int disp16 = (disp8h << 8) | disp8l;

    if (instr[6] == '1') {
      if (disp16)
        printf("%s, [%s + %d]\n", get_reg(reg, instr[7]), get_rm(rm), disp16);
      else
        printf("%s, [%s]\n", get_reg(reg, instr[7]), get_rm(rm));
    } else {
      if (disp16)
        printf("[%s + %d], %s\n", get_rm(rm), disp16, get_reg(reg, instr[7]));
      else
        printf("[%s], %s\n", get_rm(rm), get_reg(reg, instr[7]));
    }

    return;
  }

  if (strcmp(mod, "11") == 0) {
    printf("mov ");

    if (instr[6] == '1') {
      printf("%s, %s\n", get_reg(reg, instr[7]), get_reg(rm, instr[7]));
    } else {
      printf("%s, %s\n", get_reg(rm, instr[7]), get_reg(reg, instr[7]));
    }

    return;
  }
}

void handle_mov_immediate(FILE *f, char *instr) {
  char w = instr[4], reg[4];

  strncpy(reg, instr + 5, 3);

  int b1 = fgetc(f);
  unsigned int disp8l = b1 & 0xFF;

  if (w == '1') {
    int b2 = fgetc(f);
    unsigned int disp8h = b2 & 0xFF;
    unsigned int disp16 = (disp8h << 8) | disp8l;

    printf("mov ");
    printf("%s, %d\n", get_reg(reg, w), disp16);
  } else {
    printf("mov ");
    printf("%s, %d\n", get_reg(reg, w), disp8l);
  }
}

int main(int argc, char **argv) {
  if (argc != 2) {
    printf("You must provide an binary to disassemble!\n");
    exit(1);
  }

  FILE *f = fopen(argv[1], "rb");

  if (f == NULL) {
    printf("Error opening file!\n");
    exit(1);
  }

  fseek(f, 0L, SEEK_END);

  long size = ftell(f);
  int fi = 0;

  rewind(f);
  printf("bits 16\n\n");

  while (fi <= size) {
    char instruction[8], prefix[5];
    int c = fgetc(f);

    readinstruction(instruction, c);
    strncpy(prefix, instruction, 4);

    if (strcmp(prefix, "1000") == 0)
      handle_mov_reg_rm(f, instruction);
    else if (strcmp(prefix, "1011") == 0)
      handle_mov_immediate(f, instruction);
    else
      break;
  }

  fclose(f);
  return 0;
}
