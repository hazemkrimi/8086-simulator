#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"
#include "decoder.h"

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

    else if (strcmp(opcode, "01110101") == 0)
      handle_jmp_instr(f, "jnz");

    else if (strcmp(opcode, "01110100") == 0)
      handle_jmp_instr(f, "je");

    else if (strcmp(opcode, "01111100") == 0)
      handle_jmp_instr(f, "jl");

    else if (strcmp(opcode, "01111110") == 0)
      handle_jmp_instr(f, "jle");

    else if (strcmp(opcode, "01110010") == 0)
      handle_jmp_instr(f, "jb");

    else if (strcmp(opcode, "01110110") == 0)
      handle_jmp_instr(f, "jbe");

    else if (strcmp(opcode, "01111010") == 0)
      handle_jmp_instr(f, "jp");

    else if (strcmp(opcode, "01110000") == 0)
      handle_jmp_instr(f, "jo");

    else if (strcmp(opcode, "01111000") == 0)
      handle_jmp_instr(f, "js");

    else if (strcmp(opcode, "01111101") == 0)
      handle_jmp_instr(f, "jnl");

    else if (strcmp(opcode, "01111111") == 0)
      handle_jmp_instr(f, "jg");

    else if (strcmp(opcode, "01110011") == 0)
      handle_jmp_instr(f, "jnb");

    else if (strcmp(opcode, "01110111") == 0)
      handle_jmp_instr(f, "ja");

    else if (strcmp(opcode, "01111011") == 0)
      handle_jmp_instr(f, "jnp");

    else if (strcmp(opcode, "01110001") == 0)
      handle_jmp_instr(f, "jno");

    else if (strcmp(opcode, "01111001") == 0)
      handle_jmp_instr(f, "jns");

    else if (strcmp(opcode, "11100010") == 0)
      handle_jmp_instr(f, "loop");

    else if (strcmp(opcode, "11100001") == 0)
      handle_jmp_instr(f, "loopz");

    else if (strcmp(opcode, "11100000") == 0)
      handle_jmp_instr(f, "loopnz");

    else if (strcmp(opcode, "11100011") == 0)
      handle_jmp_instr(f, "jcxz");

    else {
      break;
    }
  }

  fclose(f);
  return 0;
}
