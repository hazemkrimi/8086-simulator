#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "types.h"
#include "utils.h"

struct storage {
  enum storage_type type;
  int value;
  int size;
};

static int get_location(int16_t registers[8], char *eval) {
  if (isnumber(eval))
    return atoi(eval);

  if (strstr(eval, "+") != NULL) {
    char opone[10], optwo[10];

    sscanf(eval, "%s + %s", opone, optwo);

    return get_location(registers, opone) + get_location(registers, optwo);
  } else {
    int index = get_sim_reg_index(eval);

    if (index != -1)
      return registers[index];
  }

  return -1;
}

static struct storage get_dest(int16_t registers[8], char *dest) {
  struct storage s;
  int index;

  index = get_sim_reg_index(dest);

  if (index != -1) {
    s.type = REGISTER;
    s.value = index;

    return s;
  }

  char location[30];
  int size;

  if (strstr(dest, "word") != NULL) {
    size = 16;

    sscanf(dest, "word [%[^]]]", location);
  } else if (strstr(dest, "byte") != NULL) {
    size = 8;

    sscanf(dest, "byte [%[^]]]", location);
  } else if (strstr(dest, "[") != NULL) {
    size = 16;

    sscanf(dest, "[%[^]]]", location);
  }

  index = get_location(registers, location);

  if (index == -1) {
    s.type = MEMORY;
    s.value = -1;
    s.size = size;

    return s;
  }

  s.type = MEMORY;
  s.value = index;
  s.size = size;

  return s;
}

static struct storage get_source(int16_t registers[8], char *source) {
  if (isnumber(source)) {
    struct storage s;

    s.type = IMMEDIATE;
    s.value = atoi(source);

    return s;
  }

  return get_dest(registers, source);
}

void simulate(char *oper, int16_t registers[8], int8_t memory[65536], int *ip,
              int *zero_flag, int *signed_flag, char *instr) {
  char kind[10], operand1[20], operand2[20];
  int16_t value;

  sscanf(instr, "%s %[^,], %[^\n]", kind, operand1, operand2);

  struct storage dest = get_dest(registers, operand1);
  struct storage source = get_source(registers, operand2);

  if (source.value < 0 || (strcmp(kind, "jnz") != 0 && dest.value < 0)) {
    printf("ERROR: incorrect parsing of instruction source and destination\n");
    exit(1);
  }

  if (strcmp(kind, "jnz") == 0 && *zero_flag == 0) {
    value = (int16_t)atoi(operand1);
    *ip += (int)value;

    return;
  }

  switch (source.type) {
  case IMMEDIATE:
    value = source.value;
    break;
  case REGISTER:
    value = registers[source.value];
    break;
  case MEMORY:
    if (source.size == 16)
      value = memory[source.value] | memory[source.value + 1];
    else
      value = memory[source.value];
  }

  if (strcmp(kind, "add") == 0) {
    if (dest.type == REGISTER)
      value = registers[dest.value] + value;
    else if (dest.size == 8)
      value = memory[dest.value] + (value & 0x00FF);
    else {
      value = (memory[source.value] | memory[source.value + 1]) + value;
    }
  }

  if (strcmp(kind, "sub") == 0 || strcmp(kind, "cmp") == 0) {
    if (dest.type == REGISTER)
      value = registers[dest.value] - value;
    else if (dest.size == 8)
      value = memory[dest.value] - (value & 0x00FF);
    else {
      value = (memory[dest.value] | memory[dest.value + 1]) - value;
    }
  }

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

  char reg_update[20];

  if (dest.type == REGISTER)
    sprintf(reg_update, "%s: 0x%04x -> 0x%04x", get_sim_reg_name(dest.value, 0),
            registers[dest.value], value);

  sprintf(oper, "%s IP = 0x%04x, Z = %d, S = %d", reg_update, *ip, *zero_flag,
          *signed_flag);

  if (strcmp(kind, "cmp") != 0) {
    if (dest.type == REGISTER)
      registers[dest.value] = value;
    else {
      if (dest.size == 8)
        memory[dest.value] = (int8_t)(value & 0x00FF);
      else {
        memory[dest.value] = (int8_t)(value & 0x00FF);
        memory[dest.value + 1] = (int8_t)(value & 0xFF00);
      }
    }
  }
}
