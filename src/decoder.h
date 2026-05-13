#include <stdio.h>

void decode_instr_reg_mem(char *, FILE *, char *, char *);
void decode_instr_immediate_reg_mem(char *, FILE *, char *, int);
void decode_instr_immediate_reg_acc(char *, FILE *, char *, char *);
void decode_instr_mem_accumulator(char *, FILE *, char *, char *);
void decode_jmp_instr(char *, FILE *, char *);
