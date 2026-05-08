#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char binchar(int n) {
  if (n)
    return '1';

  return '0';
}

char *getregname(char *r, int w) {
  if (strcmp(r, "000") == 0)
    return w == 1 ? "ax" : "al";

  if (strcmp(r, "001") == 0)
    return w == 1 ? "cx" : "cl";

  if (strcmp(r, "010") == 0)
    return w == 1 ? "dx" : "dl";

  if (strcmp(r, "011") == 0)
    return w == 1 ? "bx" : "bl";

  if (strcmp(r, "100") == 0)
    return w == 1 ? "sp" : "ah";

  if (strcmp(r, "101") == 0)
    return w == 1 ? "bp" : "ch";

  if (strcmp(r, "110") == 0)
    return w == 1 ? "si" : "dh";

  if (strcmp(r, "111") == 0)
    return w == 1 ? "di" : "bh";

  return "";
}

int main(int argc, char **argv) {
  if (argc != 2) {
    printf("You must provide an binary to disassemble!");
    exit(1);
  }

  FILE *f = fopen(argv[1], "rb");

  if (f == NULL) {
    printf("Error opening file!");
    exit(1);
  }

  fseek(f, 0L, SEEK_END);

  long size = ftell(f);
  int fi = 0;

  rewind(f);

  while (fi < size) {
    int b, c, d, w;
    char instruction[6];

    c = fgetc(f);

    for (int i = 0; i <= 7; ++i) {
      b = c & (1 << i);

      if (i == 0) {
        w = b ? 1 : 0;
        continue;
      }

      if (i == 1) {
        d = b ? 1 : 0;
        continue;
      }

      if (i > 1 && i <= 7) {
        instruction[abs(i - 7)] = binchar(b);
        continue;
      }
    }

    char mod[3], reg[4], rm[4];

    c = fgetc(f);

    for (int i = 0; i <= 7; ++i) {
      b = c & (1 << i);

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

    if (strcmp(mod, "11") == 0) {
      if (strcmp(instruction, "100010") == 0)
        printf("mov ");

      char *reg_text = getregname(reg, w);
      char *rm_text = getregname(rm, w);

      if (d == 1) {
        printf("%s, %s\n", reg_text, rm_text);
      } else {
        printf("%s, %s\n", rm_text, reg_text);
      }
    }

    fi += 2;
  }

  fclose(f);
  return 0;
}
