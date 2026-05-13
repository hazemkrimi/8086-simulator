#include "utils.h"
#include <stdlib.h>
#include <string.h>

char binary_to_char(int n) {
  if (n)
    return '1';

  return '0';
}

char *get_reg(char *r, char w) {
  if (strcmp(r, "000") == 0)
    return w == '1' ? "ax" : "al";

  if (strcmp(r, "001") == 0)
    return w == '1' ? "cx" : "cl";

  if (strcmp(r, "010") == 0)
    return w == '1' ? "dx" : "dl";

  if (strcmp(r, "011") == 0)
    return w == '1' ? "bx" : "bl";

  if (strcmp(r, "100") == 0)
    return w == '1' ? "sp" : "ah";

  if (strcmp(r, "101") == 0)
    return w == '1' ? "bp" : "ch";

  if (strcmp(r, "110") == 0)
    return w == '1' ? "si" : "dh";

  if (strcmp(r, "111") == 0)
    return w == '1' ? "di" : "bh";

  return "";
}

char *get_rm(char *r) {
  if (strcmp(r, "000") == 0)
    return "bx + si";

  if (strcmp(r, "001") == 0)
    return "bx + di";

  if (strcmp(r, "010") == 0)
    return "bp + si";

  if (strcmp(r, "011") == 0)
    return "bp + di";

  if (strcmp(r, "100") == 0)
    return "si";

  if (strcmp(r, "101") == 0)
    return "di";

  if (strcmp(r, "110") == 0)
    return "bp";

  if (strcmp(r, "111") == 0)
    return "bx";

  return "";
}

int get_sim_reg_index(char *reg) {
  if (strcmp(reg, "ax") == 0 || strcmp(reg, "ah") == 0)
    return 0;

  if (strcmp(reg, "bx") == 0 || strcmp(reg, "bh") == 0)
    return 1;

  if (strcmp(reg, "cx") == 0 || strcmp(reg, "ch") == 0)
    return 2;

  if (strcmp(reg, "dx") == 0 || strcmp(reg, "dh") == 0)
    return 3;

  if (strcmp(reg, "sp") == 0)
    return 4;

  if (strcmp(reg, "bp") == 0)
    return 5;

  if (strcmp(reg, "si") == 0)
    return 6;

  if (strcmp(reg, "di") == 0)
    return 7;

  return -1;
}

char *get_sim_reg_name(int reg, int low) {
  switch (reg) {
  case 0:
    if (low)
      return "ah";
    else
      return "ax";
  case 1:
    if (low)
      return "bh";
    else
      return "bx";
  case 2:
    if (low)
      return "ch";
    else
      return "cx";
  case 3:
    if (low)
      return "dh";
    else
      return "dx";
  case 4:
    return "sp";
  case 5:
    return "bp";
  case 6:
    return "si";
  case 7:
    return "di";
  }

  return "";
}

void read_opcode(char *opcode, int byte) {
  for (int i = 0; i <= 7; ++i) {
    int b = byte & (1 << i);
    opcode[abs(i - 7)] = binary_to_char(b);
  }
}

int next_byte(char * buf, int *ip) {
  return buf[(*ip)++];
}
