#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"

void decode_jmp_instr(char *result, FILE *f, char *instr) {
  int b = fgetc(f);
  int inc = b & 0xFF;

  sprintf(result, "%s %d", instr, inc);
}

void decode_instr_reg_mem(char *result, FILE *f, char *opcode, char *instr) {
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
    int disp = 0;
    unsigned int disp16;
    char strdisp16[17];

    if (strcmp(rm, "110") == 0) {
      int b = fgetc(f);
      unsigned int disp8l = b & 0xFF;
      int b1 = fgetc(f);
      unsigned int disp8h = b1 & 0xFF;

      disp16 = (disp8h << 8) | disp8l;
      disp = 1;
      sprintf(strdisp16, "%d", disp16);
    }

    if (d == '1') {
      sprintf(result, "%s %s, [%s]", instr, get_reg(reg, w),
              disp ? strdisp16 : get_rm(rm));
    } else {
      sprintf(result, "%s [%s], %s", instr, disp ? strdisp16 : get_rm(rm),
              get_reg(reg, w));
    }
  } else if (strcmp(mod, "01") == 0) {
    int b = fgetc(f);
    unsigned int disp8 = b & 0xFF;

    if (d == '1') {
      if (disp8)
        sprintf(result, "%s %s, [%s + %d]", instr, get_reg(reg, w),
                get_rm(rm), disp8);
      else
        sprintf(result, "%s %s, [%s]", instr, get_reg(reg, w), get_rm(rm));
    } else {
      if (disp8)
        sprintf(result, "%s [%s + %d], %s", instr, get_rm(rm), disp8,
                get_reg(reg, w));
      else
        sprintf(result, "%s [%s], %s", instr, get_rm(rm), get_reg(reg, w));
    }
  } else if (strcmp(mod, "10") == 0) {
    int b1 = fgetc(f);
    int b2 = fgetc(f);
    unsigned int disp8l = b1 & 0xFF;
    unsigned int disp8h = b2 & 0xFF;
    unsigned int disp16 = (disp8h << 8) | disp8l;

    if (d == '1') {
      if (disp16)
        sprintf(result, "%s %s, [%s + %d]", instr, get_reg(reg, w),
                get_rm(rm), disp16);
      else
        sprintf(result, "%s %s, [%s]", instr, get_reg(reg, w), get_rm(rm));
    } else {
      if (disp16)
        sprintf(result, "%s [%s + %d], %s", instr, get_rm(rm), disp16,
                get_reg(reg, w));
      else
        sprintf(result, "%s [%s], %s", instr, get_rm(rm), get_reg(reg, w));
    }
  } else if (strcmp(mod, "11") == 0) {
    if (d == '1') {
      sprintf(result, "%s %s, %s", instr, get_reg(reg, w), get_reg(rm, w));
    } else {
      sprintf(result, "%s %s, %s", instr, get_reg(rm, w), get_reg(reg, w));
    }
  }
}

void decode_instr_immediate_reg_acc(char *result, FILE *f, char *opcode,
                                    char *instr) {
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

    sprintf(result, "%s %s, %d", instr,
            get_reg(strcmp(instr, "mov") == 0 ? reg : "000", w), data16);
  } else {
    sprintf(result, "%s %s, %d", instr,
            get_reg(strcmp(instr, "mov") == 0 ? reg : "000", w), data8l);
  }
}

void decode_instr_immediate_reg_mem(char *result, FILE *f, char *opcode,
                                    int mov) {
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

  if (mov)
    instr = "mov";
  else if (strcmp(reg, "000") == 0)
    instr = "add";
  else if (strcmp(reg, "101") == 0)
    instr = "sub";
  else if (strcmp(reg, "111") == 0)
    instr = "cmp";
  else
    instr = "invalid";

  if (strcmp(mod, "00") == 0) {
    int disp = 0;
    unsigned int disp16;
    char strdisp16[17];

    if (strcmp(rm, "110") == 0) {
      int b = fgetc(f);
      unsigned int disp8l = b & 0xFF;
      int b1 = fgetc(f);
      unsigned int disp8h = b1 & 0xFF;

      disp16 = (disp8h << 8) | disp8l;
      disp = 1;
      sprintf(strdisp16, "%d", disp16);
    }

    int b = fgetc(f);
    unsigned int data8l = b & 0xFF;

    if ((mov || s == '0') && w == '1') {
      int b1 = fgetc(f);
      unsigned int data8h = b1 & 0xFF;
      unsigned int data16 = (data8h << 8) | data8l;

      sprintf(result, "%s %s [%s], %d", instr, w == '1' ? "word" : "byte",
              disp ? strdisp16 : get_rm(rm), data16);
    } else {
      sprintf(result, "%s %s [%s], %d", instr, w == '1' ? "word" : "byte",
              disp ? strdisp16 : get_rm(rm), data8l);
    }
  } else if (strcmp(mod, "01") == 0) {
    int b = fgetc(f);
    unsigned int disp8 = b & 0xFF;
    int b1 = fgetc(f);
    unsigned int data8l = b1 & 0xFF;

    if ((strcmp(instr, "mov") == 0 || s == '0') && w == '1') {
      int b2 = fgetc(f);
      unsigned int data8h = b2 & 0xFF;
      unsigned int data16 = (data8h << 8) | data8l;

      sprintf(result, "%s %s [%s + %d], %d", instr, w == '1' ? "word" : "byte",
              get_rm(rm), disp8, data16);
    } else {
      sprintf(result, "%s %s [%s + %d], %d", instr, w == '1' ? "word" : "byte",
              get_rm(rm), disp8, data8l);
    }
  } else if (strcmp(mod, "10") == 0) {
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

      sprintf(result, "%s %s [%s + %d], %d", instr, w == '1' ? "word" : "byte",
              get_rm(rm), disp16, data16);
    } else {
      sprintf(result, "%s %s [%s + %d], %d", instr, w == '1' ? "word" : "byte",
              get_rm(rm), disp16, data8l);
    }
  } else if (strcmp(mod, "11") == 0) {
    int b1 = fgetc(f);
    unsigned int data8l = b1 & 0xFF;

    if ((strcmp(instr, "mov") == 0 || s == '0') && w == '1') {
      int b2 = fgetc(f);
      unsigned int data8h = b2 & 0xFF;
      unsigned int data16 = (data8h << 8) | data8l;

      sprintf(result, "%s %s, %d", instr, get_reg(rm, w), data16);
    } else {
      sprintf(result, "%s %s, %d", instr, get_reg(rm, w), data8l);
    }
  }
}

void decode_instr_mem_accumulator(char *result, FILE *f, char *opcode,
                                  char *instr) {
  char w = opcode[7], d = opcode[6];
  int b1 = fgetc(f);
  unsigned int data8l = b1 & 0xFF;

  if (w == '1') {
    int b2 = fgetc(f);
    unsigned int data8h = b2 & 0xFF;
    unsigned int data16 = (data8h << 8) | data8l;

    if (d == 1)
      sprintf(result, "[%s] %s, %d", instr, get_reg("000", w), data16);
    else
      sprintf(result, "%s [%s], %d", instr, get_reg("000", w), data16);
  } else {
    if (d == 1)
      sprintf(result, "[%s] %s, %d", instr, get_reg("000", w), data8l);
    else
      sprintf(result, "%s [%s], %d", instr, get_reg("000", w), data8l);
  }
}
