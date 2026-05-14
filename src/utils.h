#ifndef UTILS

void read_opcode(char *, int);
char *get_reg(char *, char);
char *get_rm(char *);
char *get_sim_reg_name(int, int);
char binary_to_char(int);
int get_sim_reg_index(char *);
int next_byte(char *, int *);
int isnumber(char *);

#endif
