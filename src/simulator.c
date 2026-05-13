#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"

void simulate(char *oper, int16_t registers[8], char *instr) {
  char kind[10], operand1[10], operand2[10];

  sscanf(instr, "%s %[^,], %s", kind, operand1, operand2);

  if (strcmp(kind, "mov") == 0) {
    int dest_index = get_sim_reg_index(operand1);
    int source_index = get_sim_reg_index(operand2);

    if (source_index == -1) {
      int16_t value = (int16_t)atoi(operand2);

      sprintf(oper, "%s: 0x%01x -> 0x%01x", get_sim_reg_name(dest_index, 0),
              registers[dest_index], value);
      registers[dest_index] = value;
    } else {
      sprintf(oper, "%s: 0x%01x -> 0x%01x", get_sim_reg_name(dest_index, 0),
              registers[dest_index], registers[source_index]);
      registers[dest_index] = registers[source_index];
    }
  }
}
