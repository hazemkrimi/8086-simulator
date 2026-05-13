#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "decoder.h"
#include "simulator.h"
#include "utils.h"

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
  int fi = 0, zero_flag = 0, signed_flag = 0;
  char *instr = (char *)malloc(sizeof(char) * 100);
  char *oper = (char *)malloc(sizeof(char) * 100);

  rewind(f);

  int16_t registers[8] = {0, 0, 0, 0, 0, 0, 0, 0};

  while (fi <= size) {
    char opcode[8], prefix[5];
    int c = fgetc(f);

    read_opcode(opcode, c);
    strncpy(prefix, opcode, 4);

    if (strcmp(prefix, "1000") == 0 && opcode[4] == '1')
      decode_instr_reg_mem(instr, f, opcode, "mov");

    else if (strcmp(prefix, "1000") == 0 && opcode[4] == '0')
      decode_instr_immediate_reg_mem(instr, f, opcode, 0);

    else if (strcmp(prefix, "1100") == 0)
      decode_instr_immediate_reg_mem(instr, f, opcode, 1);

    else if (strcmp(prefix, "1011") == 0)
      decode_instr_immediate_reg_acc(instr, f, opcode, "mov");

    else if (strcmp(prefix, "1010") == 0)
      decode_instr_mem_accumulator(instr, f, opcode, "mov");

    else if (strcmp(prefix, "0000") == 0 && opcode[5] == '0')
      decode_instr_reg_mem(instr, f, opcode, "add");

    else if (strcmp(prefix, "0000") == 0 && opcode[5] == '1')
      decode_instr_immediate_reg_acc(instr, f, opcode, "add");

    else if (strcmp(prefix, "0010") == 0 && opcode[5] == '0')
      decode_instr_reg_mem(instr, f, opcode, "sub");

    else if (strcmp(prefix, "0010") == 0 && opcode[5] == '1')
      decode_instr_immediate_reg_acc(instr, f, opcode, "sub");

    else if (strcmp(prefix, "0011") == 0 && opcode[5] == '0')
      decode_instr_reg_mem(instr, f, opcode, "cmp");

    else if (strcmp(prefix, "0011") == 0 && opcode[5] == '1')
      decode_instr_immediate_reg_acc(instr, f, opcode, "cmp");

    else if (strcmp(opcode, "01110101") == 0)
      decode_jmp_instr(instr, f, "jnz");

    else if (strcmp(opcode, "01110100") == 0)
      decode_jmp_instr(instr, f, "je");

    else if (strcmp(opcode, "01111100") == 0)
      decode_jmp_instr(instr, f, "jl");

    else if (strcmp(opcode, "01111110") == 0)
      decode_jmp_instr(instr, f, "jle");

    else if (strcmp(opcode, "01110010") == 0)
      decode_jmp_instr(instr, f, "jb");

    else if (strcmp(opcode, "01110110") == 0)
      decode_jmp_instr(instr, f, "jbe");

    else if (strcmp(opcode, "01111010") == 0)
      decode_jmp_instr(instr, f, "jp");

    else if (strcmp(opcode, "01110000") == 0)
      decode_jmp_instr(instr, f, "jo");

    else if (strcmp(opcode, "01111000") == 0)
      decode_jmp_instr(instr, f, "js");

    else if (strcmp(opcode, "01111101") == 0)
      decode_jmp_instr(instr, f, "jnl");

    else if (strcmp(opcode, "01111111") == 0)
      decode_jmp_instr(instr, f, "jg");

    else if (strcmp(opcode, "01110011") == 0)
      decode_jmp_instr(instr, f, "jnb");

    else if (strcmp(opcode, "01110111") == 0)
      decode_jmp_instr(instr, f, "ja");

    else if (strcmp(opcode, "01111011") == 0)
      decode_jmp_instr(instr, f, "jnp");

    else if (strcmp(opcode, "01110001") == 0)
      decode_jmp_instr(instr, f, "jno");

    else if (strcmp(opcode, "01111001") == 0)
      decode_jmp_instr(instr, f, "jns");

    else if (strcmp(opcode, "11100010") == 0)
      decode_jmp_instr(instr, f, "loop");

    else if (strcmp(opcode, "11100001") == 0)
      decode_jmp_instr(instr, f, "loopz");

    else if (strcmp(opcode, "11100000") == 0)
      decode_jmp_instr(instr, f, "loopnz");

    else if (strcmp(opcode, "11100011") == 0)
      decode_jmp_instr(instr, f, "jcxz");

    else
      break;

    simulate(oper, registers, &zero_flag, &signed_flag, instr);
    printf("%s ; %s\n", instr, oper);
  }

  fclose(f);
  free(instr);
  free(oper);
  printf("\n");

  for (int i = 0; i <= 7; ++i)
    printf("%s: 0x%04x (%d)\n", get_sim_reg_name(i, 0),
           (unsigned short)registers[i], (unsigned short)registers[i]);

  printf("flags: Z = %d, S = %d\n", zero_flag, signed_flag);

  return 0;
}
