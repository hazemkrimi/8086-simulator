#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "types.h"
#include "utils.h"

struct info {
  enum storage_type type;
  union {
    int ea;
    int accumulator;
  } details;
};

static struct info get_info(char *operand) {
  struct info i;

  if (isnumber(operand)) {
    i.type = IMMEDIATE;

    return i;
  }

  if (get_sim_reg_index(operand) > -1) {
    i.type = REGISTER;

    if (strcmp(operand, "ax") == 0) {
      i.details.accumulator = 1;
    }

    return i;
  }

  char content[30];

  if (strstr(operand, "word") != NULL)
    sscanf(operand, "word [%[^]]]", content);
  else
    sscanf(operand, "[%[^]]]", content);

  if (isnumber(content)) {
    i.type = MEMORY;
    i.details.ea = 6;

    return i;
  }

  // NOTE(hazem): Unfinished logic for handling effective address calculation
  if (strstr(content, "+") != NULL) {
    char *eac1 = strtok(content, " + ");
    char *eac2 = strtok(NULL, " + ");

    if (eac1 == NULL || eac2 == NULL) {
      printf(
          "ERROR: one of the memory operands is NULL when estimating clocks\n");
      exit(1);
    }

    i.type = MEMORY;
    i.details.ea = 9;

    return i;
  }

  switch (get_sim_reg_index(content)) {
  case 1:
  case 5:
  case 6:
  case 7:
    i.type = MEMORY;
    i.details.ea = 5;
  }

  return i;
}

void estimate(char *est, int *clocks, char *instr) {
  char kind[10], operand1[20], operand2[20], estimation[26];
  int cycles = 0, ea = 0;

  sscanf(instr, "%s %[^,], %[^\n]", kind, operand1, operand2);

  struct info dest = get_info(operand1);
  struct info source = get_info(operand2);

  if (dest.type == REGISTER && source.type == IMMEDIATE)
    cycles = 4;
  else if (dest.type == REGISTER && source.type == REGISTER)
    cycles = strcmp(kind, "add") == 0 ? 3 : 2;
  else if (dest.type == REGISTER && source.type == MEMORY)
    cycles = 8, ea = source.details.ea;
  else if (dest.type == MEMORY && source.type == REGISTER)
    cycles = strcmp(kind, "add") == 0 ? 16 : 9, ea = dest.details.ea;

  *clocks += (cycles + ea);

  if (ea)
    sprintf(estimation, "clocks: +%d = %d, (%d + %dea)", cycles + ea, *clocks,
            cycles, ea);
  else
    sprintf(estimation, "clocks: +%d = %d", cycles, *clocks);

  sprintf(est, "%s", estimation);
}
