#include <stdlib.h>
#include <string.h>

char binchar(int n) {
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

void readinstruction(char *instruction, int byte) {
  for (int i = 0; i <= 7; ++i) {
    int b = byte & (1 << i);
    instruction[abs(i - 7)] = binchar(b);
  }
}
