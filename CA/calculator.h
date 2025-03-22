#pragma once

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "instruction.h"

extern inst InstMemory[0x1000000];
extern uint32_t  opcode, pc;
extern uint16_t op_types, pflag;
extern uint32_t* op1, * op2, * op3;
extern inst IR;
extern uint32_t reg[10];

void trim(char* origin);
int hex_to_dec(char* str);

void init(void);
void load(char* program);
void fetch(void);
void decode(void);
void execute(void);

void reset_reg(void);
int get_reg_index(int);
void print_register(void);


int integrity_check(inst ins);
