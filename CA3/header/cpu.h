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

typedef struct IFID_latch {
    unsigned int valid;               
    unsigned int next_PC;          
    unsigned int data;             

    unsigned int BH_found;      
    unsigned int BH_index;
    unsigned int pretaken;
} IFID_latch;

typedef struct _IDEX_latch {
    CU control;
    unsigned int valid;                  
    unsigned int next_PC;            
    unsigned int read_data1;
    unsigned int read_data2; 

    unsigned int opcode;         
    unsigned int rs;                  
    unsigned int rt;                  
    unsigned int rd;                  
    unsigned int addr;   
    
    unsigned int imm;             
    unsigned int sign_extened_data;
    unsigned int zero_extened_data;
    unsigned int lui_shifted_data;

    unsigned int shamt;            
    unsigned int funct;              

    unsigned int BH_found;      
    unsigned int BH_index;
    unsigned int pretaken;
} IDEX_latch;

typedef struct _EXMEM_latch {
    CU control;
    ALU_flag flag;   
    unsigned int valid;                
    unsigned int next_PC;              
    unsigned int branch_target;       
    unsigned int alu_result;    
    unsigned int read_data1;  
    unsigned int read_data2;  
    unsigned int rd;                  
} EXMEM_latch;

typedef struct _MEMWB_latch {
    CU control;
    unsigned int valid;                     
    unsigned int next_PC;                
    unsigned int read_data;      
    unsigned int alu_result;     
    unsigned int rd;                     
} MEMWB_latch;

typedef struct _branch_history {
    unsigned int branch_inst_addr;        
    unsigned int branch_target_addr;    
    unsigned int prediction_bit;                 
} BHU;

typedef struct _CPU{
    Register reg;
  	ALU alu;
    ALU_CU acu;
    CU cu;

    IFID_latch IFID[2];
    IDEX_latch IDEX[2];
    EXMEM_latch EXMEM[2];
    MEMWB_latch MEMWB[2];

    unsigned int data_hazard_option;
    unsigned int control_hazard_option;
    unsigned int foward_unit[2];
    BHU BH_unit[0x100];

    unsigned int* fwd_mtr[2];
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
void H_detect(CPU* cpu);
void latch_update(CPU* cpu);

void detect_wait_execution(CPU* cpu, unsigned int branch_taken, unsigned int addr);

void ANT_prediction(CPU* cpu);
void ANT_execution(CPU* cpu, unsigned int branch_taken, unsigned int addr);

void ALT_prediction(CPU* cpu);
void ALT_execution(CPU* cpu, unsigned int branch_taken, unsigned int addr);

void LTP_prediction(CPU* cpu);
void LTP_execution(CPU* cpu, unsigned int branch_taken, unsigned int addr);




#endif