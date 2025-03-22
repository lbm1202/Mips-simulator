#ifndef _CPU_H
#define _CPU_H

#include "acu.h"
#include "alu.h"
#include "cu.h"
#include "memory.h"
#include "register.h"

typedef struct _R_type{
	unsigned int funct: 6;
	unsigned int shamt: 5;
	unsigned int rd: 5;
	unsigned int rt: 5;
	unsigned int rs: 5;
	unsigned int opcode: 6;
}R_type;

typedef struct _I_type{
	unsigned int imm: 16;
	unsigned int rt: 5;
	unsigned int rs: 5;
	unsigned int opcode: 6;
}I_type;

typedef struct _J_type{
	unsigned int addr: 26;
	unsigned int opcode: 6;
}J_type;

typedef struct _CPU{
    Register reg;
  	ALU alu;
    ALU_CU acu;
    CU cu;
    
    unsigned int* write_reg_mux[3];
    unsigned int* alu_src_mux[4];
    unsigned int alu_result_mux[2];
    unsigned int load_mux[4];
    unsigned int pc_mem_mux[2];
    unsigned int branch_mux[4];
    unsigned int branch_taken_mux[2];
    unsigned int jump_mux[2];
    unsigned int jump_reg_mux[2];
    
     
    unsigned int pc_adder;
    unsigned int branch_adder;
    unsigned int jal_adder;

    unsigned int zero_extened_data;
    unsigned int sign_extened_data;
    unsigned int lui_shifted_data;
}CPU;


void init_cpu(CPU* cpu);
void fetch(CPU* cpu, Memory* mem);
void decode(CPU* cpu);
void execute(CPU* cpu);
void memory_operation(CPU* cpu, Memory* mem);
void writeback(CPU* cpu, Memory* mem);
void PC_operation(CPU* cpu);



#endif