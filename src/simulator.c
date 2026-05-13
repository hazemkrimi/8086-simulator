#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"

void simulate(char *oper, int16_t registers[8], int *ip, int *zero_flag,
              int *signed_flag, char *instr) {
  char kind[10], operand1[10], operand2[10];
  int16_t value;

  sscanf(instr, "%s %[^,], %s", kind, operand1, operand2);

  int dest_index = get_sim_reg_index(operand1);
  int source_index = get_sim_reg_index(operand2);

  if (strcmp(kind, "jnz") == 0 && *zero_flag == 0) {
    value = (int16_t)atoi(operand1);
    *ip += (int)value;

    return;
  }

  if (source_index == -1)
    value = (int16_t)atoi(operand2);
  else
    value = registers[source_index];

  if (strcmp(kind, "add") == 0)
    value += registers[dest_index];

  if (strcmp(kind, "sub") == 0 || strcmp(kind, "cmp") == 0)
    value = registers[dest_index] - value;

  if (strcmp(kind, "mov") != 0) {
    if (!value)
      *zero_flag = 1;
    else
      *zero_flag = 0;

    if (value & 0x8000)
      *signed_flag = 1;
    else
      *signed_flag = 0;
  }

  if (strcmp(kind, "cmp") != 0 && strcmp(kind, "jnz") != 0)
    sprintf(oper, "%s: 0x%04x -> 0x%04x, IP = 0x%04x, Z = %d, S = %d",
            get_sim_reg_name(dest_index, 0),
            (unsigned short)registers[dest_index], (unsigned short)value, *ip,
            *zero_flag, *signed_flag);
  else
    sprintf(oper, "IP = 0x%04x, Z = %d, S = %d", *ip, *zero_flag, *signed_flag);

  if (strcmp(kind, "cmp") != 0)
    registers[dest_index] = value;
}
