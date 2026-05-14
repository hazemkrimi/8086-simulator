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

  long length = ftell(f);
  int ip = 0, zero_flag = 0, signed_flag = 0;

  char *buf = (char *)malloc(sizeof(char) * length);
  char *instr = (char *)malloc(sizeof(char) * 100);
  char *oper = (char *)malloc(sizeof(char) * 100);

  rewind(f);
  fread(buf, 1, length, f);
  fclose(f);

  int16_t registers[8] = {0, 0, 0, 0, 0, 0, 0, 0};
  int8_t memory[65536];

  for (int i = 0; i < 65536; ++i)
    memory[i] = 0;

  while (ip < length) {
    char opcode[8], prefix[5];
    int c = next_byte(buf, &ip);

    read_opcode(opcode, c);
    strncpy(prefix, opcode, 4);

    if (strcmp(prefix, "1000") == 0 && opcode[4] == '1')
      decode_instr_reg_mem(instr, buf, &ip, opcode, "mov");

    else if (strcmp(prefix, "1000") == 0 && opcode[4] == '0')
      decode_instr_immediate_reg_mem(instr, buf, &ip, opcode, 0);

    else if (strcmp(prefix, "1100") == 0)
      decode_instr_immediate_reg_mem(instr, buf, &ip, opcode, 1);

    else if (strcmp(prefix, "1011") == 0)
      decode_instr_immediate_reg_acc(instr, buf, &ip, opcode, "mov");

    else if (strcmp(prefix, "1010") == 0)
      decode_instr_mem_accumulator(instr, buf, &ip, opcode, "mov");

    else if (strcmp(prefix, "0000") == 0 && opcode[5] == '0')
      decode_instr_reg_mem(instr, buf, &ip, opcode, "add");

    else if (strcmp(prefix, "0000") == 0 && opcode[5] == '1')
      decode_instr_immediate_reg_acc(instr, buf, &ip, opcode, "add");

    else if (strcmp(prefix, "0010") == 0 && opcode[5] == '0')
      decode_instr_reg_mem(instr, buf, &ip, opcode, "sub");

    else if (strcmp(prefix, "0010") == 0 && opcode[5] == '1')
      decode_instr_immediate_reg_acc(instr, buf, &ip, opcode, "sub");

    else if (strcmp(prefix, "0011") == 0 && opcode[5] == '0')
      decode_instr_reg_mem(instr, buf, &ip, opcode, "cmp");

    else if (strcmp(prefix, "0011") == 0 && opcode[5] == '1')
      decode_instr_immediate_reg_acc(instr, buf, &ip, opcode, "cmp");

    else if (strcmp(opcode, "01110101") == 0)
      decode_jmp_instr(instr, buf, &ip, "jnz");

    else if (strcmp(opcode, "01110100") == 0)
      decode_jmp_instr(instr, buf, &ip, "je");

    else if (strcmp(opcode, "01111100") == 0)
      decode_jmp_instr(instr, buf, &ip, "jl");

    else if (strcmp(opcode, "01111110") == 0)
      decode_jmp_instr(instr, buf, &ip, "jle");

    else if (strcmp(opcode, "01110010") == 0)
      decode_jmp_instr(instr, buf, &ip, "jb");

    else if (strcmp(opcode, "01110110") == 0)
      decode_jmp_instr(instr, buf, &ip, "jbe");

    else if (strcmp(opcode, "01111010") == 0)
      decode_jmp_instr(instr, buf, &ip, "jp");

    else if (strcmp(opcode, "01110000") == 0)
      decode_jmp_instr(instr, buf, &ip, "jo");

    else if (strcmp(opcode, "01111000") == 0)
      decode_jmp_instr(instr, buf, &ip, "js");

    else if (strcmp(opcode, "01111101") == 0)
      decode_jmp_instr(instr, buf, &ip, "jnl");

    else if (strcmp(opcode, "01111111") == 0)
      decode_jmp_instr(instr, buf, &ip, "jg");

    else if (strcmp(opcode, "01110011") == 0)
      decode_jmp_instr(instr, buf, &ip, "jnb");

    else if (strcmp(opcode, "01110111") == 0)
      decode_jmp_instr(instr, buf, &ip, "ja");

    else if (strcmp(opcode, "01111011") == 0)
      decode_jmp_instr(instr, buf, &ip, "jnp");

    else if (strcmp(opcode, "01110001") == 0)
      decode_jmp_instr(instr, buf, &ip, "jno");

    else if (strcmp(opcode, "01111001") == 0)
      decode_jmp_instr(instr, buf, &ip, "jns");

    else if (strcmp(opcode, "11100010") == 0)
      decode_jmp_instr(instr, buf, &ip, "loop");

    else if (strcmp(opcode, "11100001") == 0)
      decode_jmp_instr(instr, buf, &ip, "loopz");

    else if (strcmp(opcode, "11100000") == 0)
      decode_jmp_instr(instr, buf, &ip, "loopnz");

    else if (strcmp(opcode, "11100011") == 0)
      decode_jmp_instr(instr, buf, &ip, "jcxz");

    simulate(oper, registers, memory, &ip, &zero_flag, &signed_flag, instr);
    printf("%s ; %s\n", instr, oper);
  }

  free(instr);
  free(oper);
  printf("\n");

  for (int i = 0; i <= 7; ++i)
    printf("%s: 0x%04x (%d)\n", get_sim_reg_name(i, 0),
           (unsigned short)registers[i], (unsigned short)registers[i]);

  printf("flags: Z = %d, S = %d\n", zero_flag, signed_flag);
  printf("ip: 0x%04x (%d)\n", ip, ip);

  return 0;
}
