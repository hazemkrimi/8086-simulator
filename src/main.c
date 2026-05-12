#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"

void handle_instr_reg_mem(FILE *f, char *opcode, char *instr) {
  char mod[3], reg[4], rm[4];
  char d = opcode[6], w = opcode[7];
  int c = fgetc(f);

  for (int i = 0; i <= 7; ++i) {
    int b = c & (1 << i);

    if (i >= 0 && i < 3) {
      rm[abs(i - 2)] = binary_to_char(b);
      continue;
    }

    if (i >= 3 && i < 6) {
      reg[abs(i - 5)] = binary_to_char(b);
      continue;
    }

    if (i > 5) {
      mod[abs(i - 7)] = binary_to_char(b);
      continue;
    }
  }

  mod[2] = '\0';
  reg[3] = '\0';
  rm[3] = '\0';

  if (strcmp(mod, "00") == 0) {
    if (d == '1') {
      printf("%s %s, [%s]\n", instr, get_reg(reg, w), get_rm(rm));
    } else {
      printf("%s [%s], %s\n", instr, get_rm(rm), get_reg(reg, w));
    }

    return;
  }

  if (strcmp(mod, "01") == 0) {
    int b = fgetc(f);
    unsigned int disp8 = b & 0xFF;

    if (d == '1') {
      if (disp8)
        printf("%s %s, [%s + %d]\n", instr, get_reg(reg, w), get_rm(rm), disp8);
      else
        printf("%s %s, [%s]\n", instr, get_reg(reg, w), get_rm(rm));
    } else {
      if (disp8)
        printf("%s [%s + %d], %s\n", instr, get_rm(rm), disp8, get_reg(reg, w));
      else
        printf("%s [%s], %s\n", instr, get_rm(rm), get_reg(reg, w));
    }

    return;
  }

  if (strcmp(mod, "10") == 0) {
    int b1 = fgetc(f);
    int b2 = fgetc(f);
    unsigned int disp8l = b1 & 0xFF;
    unsigned int disp8h = b2 & 0xFF;
    unsigned int disp16 = (disp8h << 8) | disp8l;

    if (d == '1') {
      if (disp16)
        printf("%s %s, [%s + %d]\n", instr, get_reg(reg, w), get_rm(rm),
               disp16);
      else
        printf("%s %s, [%s]\n", instr, get_reg(reg, w), get_rm(rm));
    } else {
      if (disp16)
        printf("%s [%s + %d], %s\n", instr, get_rm(rm), disp16,
               get_reg(reg, w));
      else
        printf("%s [%s], %s\n", instr, get_rm(rm), get_reg(reg, w));
    }

    return;
  }

  if (strcmp(mod, "11") == 0) {
    if (d == '1') {
      printf("%s %s, %s\n", instr, get_reg(reg, w), get_reg(rm, w));
    } else {
      printf("%s %s, %s\n", instr, get_reg(rm, w), get_reg(reg, w));
    }

    return;
  }
}

void handle_instr_immediate_reg_acc(FILE *f, char *opcode, char *instr) {
  char w, reg[4];

  if (strcmp(instr, "mov") == 0) {
    w = opcode[4];
    strncpy(reg, opcode + 5, 3);
  } else {
    w = opcode[7];
  }

  int b1 = fgetc(f);
  unsigned int data8l = b1 & 0xFF;

  if (w == '1') {
    int b2 = fgetc(f);
    unsigned int data8h = b2 & 0xFF;
    unsigned int data16 = (data8h << 8) | data8l;

    printf("%s %s, %d\n", instr,
           get_reg(strcmp(instr, "mov") == 0 ? reg : "000", w), data16);
  } else {
    printf("%s %s, %d\n", instr,
           get_reg(strcmp(instr, "mov") == 0 ? reg : "000", w), data8l);
  }
}

void handle_instr_immediate_reg_mem(FILE *f, char *opcode, int mov) {
  char mod[3], reg[4], rm[4], *instr;
  char s = opcode[6], w = opcode[7];
  int c = fgetc(f);

  for (int i = 0; i <= 7; ++i) {
    int b = c & (1 << i);

    if (i >= 0 && i < 3) {
      rm[abs(i - 2)] = binary_to_char(b);
      continue;
    }

    if (i >= 3 && i < 6) {
      reg[abs(i - 5)] = binary_to_char(b);
      continue;
    }

    if (i > 5) {
      mod[abs(i - 7)] = binary_to_char(b);
      continue;
    }
  }

  mod[2] = '\0';
  reg[3] = '\0';
  rm[3] = '\0';

  if (mov) {
    instr = "mov";
  } else if (strcmp(reg, "000") == 0) {
    instr = "add";
  } else if (strcmp(reg, "101") == 0) {
    instr = "sub";
  } else if (strcmp(reg, "111") == 0) {
    instr = "cmp";
  } else {
    instr = "invalid";
  }

  if (strcmp(mod, "00") == 0) {
    int b1 = fgetc(f);
    unsigned int data8l = b1 & 0xFF;

    if ((mov || s == '0') && w == '1') {
      int b2 = fgetc(f);
      unsigned int data8h = b2 & 0xFF;
      unsigned int data16 = (data8h << 8) | data8l;

      printf("%s %s [%s], %d\n", instr, w == '1' ? "word" : "byte", get_rm(rm),
             data16);
    } else {
      printf("%s %s [%s], %d\n", instr, w == '1' ? "word" : "byte", get_rm(rm),
             data8l);
    }

    return;
  }

  if (strcmp(mod, "01") == 0) {
    int b = fgetc(f);
    unsigned int disp8 = b & 0xFF;
    int b1 = fgetc(f);
    unsigned int data8l = b1 & 0xFF;

    if ((strcmp(instr, "mov") == 0 || s == '0') && w == '1') {
      int b2 = fgetc(f);
      unsigned int data8h = b2 & 0xFF;
      unsigned int data16 = (data8h << 8) | data8l;

      printf("%s %s [%s + %d], %d\n", instr, w == '1' ? "word" : "byte",
             get_rm(rm), disp8, data16);
    } else {
      printf("%s %s [%s + %d], %d\n", instr, w == '1' ? "word" : "byte",
             get_rm(rm), disp8, data8l);
    }

    return;
  }

  if (strcmp(mod, "10") == 0) {
    int b = fgetc(f);
    unsigned int disp8l = b & 0xFF;
    int b1 = fgetc(f);
    unsigned int disp8h = b1 & 0xFF;
    unsigned int disp16 = (disp8h << 8) | disp8l;
    int b2 = fgetc(f);
    unsigned int data8l = b2 & 0xFF;

    if ((strcmp(instr, "mov") == 0 || s == '0') && w == '1') {
      int b3 = fgetc(f);
      unsigned int data8h = b3 & 0xFF;
      unsigned int data16 = (data8h << 8) | data8l;

      printf("%s %s [%s + %d], %d\n", instr, w == '1' ? "word" : "byte",
             get_rm(rm), disp16, data16);
    } else {
      printf("%s %s [%s + %d], %d\n", instr, w == '1' ? "word" : "byte",
             get_rm(rm), disp16, data8l);
    }

    return;
  }

  if (strcmp(mod, "11") == 0) {
    int b1 = fgetc(f);
    unsigned int data8l = b1 & 0xFF;

    if ((strcmp(instr, "mov") == 0 || s == '0') && w == '1') {
      int b2 = fgetc(f);
      unsigned int data8h = b2 & 0xFF;
      unsigned int data16 = (data8h << 8) | data8l;

      printf("%s %s, %d\n", instr, get_reg(rm, w), data16);
    } else {
      printf("%s %s, %d\n", instr, get_reg(rm, w), data8l);
    }

    return;
  }
}

void handle_instr_mem_accumulator(FILE *f, char *opcode, char *instr) {
  char w = opcode[7], d = opcode[6];

  int b1 = fgetc(f);
  unsigned int data8l = b1 & 0xFF;

  if (w == '1') {
    int b2 = fgetc(f);
    unsigned int data8h = b2 & 0xFF;
    unsigned int data16 = (data8h << 8) | data8l;

    if (d == 1)
      printf("[%s] %s, %d\n", instr, get_reg("000", w), data16);
    else
      printf("%s [%s], %d\n", instr, get_reg("000", w), data16);
  } else {
    if (d == 1)
      printf("[%s] %s, %d\n", instr, get_reg("000", w), data8l);
    else
      printf("%s [%s], %d\n", instr, get_reg("000", w), data8l);
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
    char opcode[8], prefix[5];
    int c = fgetc(f);

    read_opcode(opcode, c);
    strncpy(prefix, opcode, 4);

    if (strcmp(prefix, "1000") == 0 && opcode[4] == '1')
      handle_instr_reg_mem(f, opcode, "mov");

    else if (strcmp(prefix, "1000") == 0 && opcode[4] == '0')
      handle_instr_immediate_reg_mem(f, opcode, 0);

    else if (strcmp(prefix, "1100") == 0)
      handle_instr_immediate_reg_mem(f, opcode, 1);

    else if (strcmp(prefix, "1011") == 0)
      handle_instr_immediate_reg_acc(f, opcode, "mov");

    else if (strcmp(prefix, "1010") == 0)
      handle_instr_mem_accumulator(f, opcode, "mov");

    else if (strcmp(prefix, "0000") == 0 && opcode[5] == '0')
      handle_instr_reg_mem(f, opcode, "add");

    else if (strcmp(prefix, "0000") == 0 && opcode[5] == '1')
      handle_instr_immediate_reg_acc(f, opcode, "add");

    else if (strcmp(prefix, "0010") == 0 && opcode[5] == '0')
      handle_instr_reg_mem(f, opcode, "sub");

    else if (strcmp(prefix, "0010") == 0 && opcode[5] == '1')
      handle_instr_immediate_reg_acc(f, opcode, "sub");

    else if (strcmp(prefix, "0011") == 0 && opcode[5] == '0')
      handle_instr_reg_mem(f, opcode, "cmp");

    else if (strcmp(prefix, "0011") == 0 && opcode[5] == '1')
      handle_instr_immediate_reg_acc(f, opcode, "cmp");

    else {
      break;
    }
  }

  fclose(f);
  return 0;
}
