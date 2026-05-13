#include <stdio.h>

void handle_instr_reg_mem(FILE *, char *, char *);
void handle_instr_immediate_reg_mem(FILE *, char *, int);
void handle_instr_immediate_reg_acc(FILE *, char *, char *);
void handle_instr_mem_accumulator(FILE *, char *, char *);
void handle_jmp_instr(FILE *, char *);
