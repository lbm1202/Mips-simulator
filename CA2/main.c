#include "header/cpu.h"
#include "header/memory.h"
#include <stdio.h>
#include <stdlib.h>
#define MEMSIZE 0xFFFFFFFF

void print_fetch(CPU* cpu);
void print_decode(CPU* cpu);
void print_mem_op(CPU* cpu);
void print_writeback(CPU* cpu, int previous_pc);
void print_result(CPU* cpu);


int cycle = 0, R_num = 0, I_num = 0, J_num = 0, branch_num = 0, mem_num = 0;


int main(void){
    int tmp_pc = 0;
    char dir[100] = "input/input1/input.bin";
    printf("choose input file(1~5) : "); scanf("%c",dir+11);
	CPU cpu;
	Memory memory;
    
    init_cpu(&cpu);
    init_memory(&memory, MEMSIZE , 0x0,0x00100000, 0x00100001,0x01001000);

    if(load_program(&memory, dir)) return 0;

    while( cpu.reg.PC != 0xFFFFFFFF){
        fetch(&cpu, &memory);
        print_fetch(&cpu);

        decode(&cpu);
        print_decode(&cpu);
        
        execute(&cpu);

        memory_operation(&cpu, &memory);
        print_mem_op(&cpu);

        tmp_pc = cpu.reg.PC;
        writeback(&cpu, &memory);
        print_writeback(&cpu, tmp_pc);
    }
    print_result(&cpu);
    
    return 0;
    
}

void print_fetch(CPU* cpu){
    printf("Cycle[%d] (PC: 0x%x)\n", ++cycle, cpu->reg.PC);
    printf("[Fetch instruction] ");
	printf("0x%08x\t pc: 0x%x\n", cpu->reg.IR.data, cpu->reg.PC);
}

void print_decode(CPU* cpu){
    printf("[Decode instruction] type: ");
	if (cpu->reg.IR.data == 0){
		printf("Nop\n");
	}
	else if (cpu->reg.IR.opcode == R){
		printf("R\n");
		printf("\topcode: 0x%x, ", cpu->reg.IR.opcode);
		printf("rs: 0x%x (R[%d]=0x%x), ", cpu->reg.IR.rs, cpu->reg.IR.rs, cpu->reg.gp_reg[cpu->reg.IR.rs]);
		printf("rt: 0x%x (R[%d]=0x%x), ", cpu->reg.IR.rt, cpu->reg.IR.rt, cpu->reg.gp_reg[cpu->reg.IR.rt]);
		printf("rd: 0x%x (%d), ", cpu->reg.IR.rd, cpu->reg.IR.rd);
		printf("shamt: 0x%x, ", cpu->reg.IR.shamt);
		printf("funct: 0x%x\n", cpu->reg.IR.funct);
		R_num++;
	}
	else if(cpu->reg.IR.opcode == J || cpu->reg.IR.opcode == JAL){
		printf("J\n\t");
		printf("opcode: 0x%x, ", cpu->reg.IR.opcode);
		printf("address: 0x%x\n", cpu->reg.IR.addr);
		J_num++;
	}
	else{
		printf("I\n\t");
		printf("opcode: 0x%x, ", cpu->reg.IR.opcode);
		printf("rs: 0x%x (R[%d]=0x%x), ", cpu->reg.IR.rs, cpu->reg.IR.rs, cpu->reg.gp_reg[cpu->reg.IR.rs]);
		printf("rt: 0x%x (R[%d]=0x%x), ", cpu->reg.IR.rt, cpu->reg.IR.rt, cpu->reg.gp_reg[cpu->reg.IR.rt]);
		printf("imm: 0x%x\n", cpu->reg.IR.imm);
		I_num++;
	}
}

void print_mem_op(CPU* cpu){
    if(cpu->cu.mem_write){
        printf("[Store] ");
        printf("Mem[0x%08x] <- R[%d] = 0x%x\n", cpu->alu.result, *(cpu->reg.reg_file.read_reg2) , cpu->reg.reg_file.read_data2);
        mem_num++;
    }
}

void print_writeback(CPU* cpu, int previous_pc){
    if (cpu->cu.mem_to_reg ){
		printf("[Load] ");
		printf("R[%d] <- Mem[0x%08x] = 0x%x\n", *(cpu->reg.reg_file.write_reg), cpu->alu.result, cpu->alu_result_mux[ cpu->cu.mem_to_reg ]);
		mem_num++;
	}
	else if (*(cpu->reg.reg_file.reg_write) && *(cpu->reg.reg_file.write_reg)){
		printf("[Write Back] ");
		printf("R[%d] <- 0x%x\n", *(cpu->reg.reg_file.write_reg), cpu->reg.reg_file.write_data);
	}

    printf("[PC Update] ");
	printf("PC <- 0x%08x = ", cpu->jump_reg_mux[ cpu->cu.jr ]);
	if (cpu->cu.jr){
		printf("R[%d]: 0x%08x\n", *(cpu->reg.reg_file.read_reg1), cpu->reg.reg_file.read_data1);
	}
	else if (cpu->cu.jump){
		printf("Jump address\n");
	}
	else if (cpu->cu.branch ? cpu->branch_mux[cpu->cu.extra & 0x3] : 0){
		printf("Branch address\n");
		branch_num++;
	}
	else{
		printf("0x%08x+4\n", previous_pc);
	}
	printf("\n");
}

void print_result(CPU* cpu){
    printf("\n===========================PROGRAM RESULT=============================\n");
	printf("Return value (R[2]) :\t\t\t%d\n", cpu->reg.gp_reg[2]);
	printf("Total Cycle :\t\t\t\t%d\n", cycle);
	printf("Executed 'R' instruction :\t\t%d\n", R_num);
	printf("Executed 'I' instruction :\t\t%d\n", I_num);
	printf("Executed 'J' instruction :\t\t%d\n", J_num);
	printf("Number of Branch Taken :\t\t%d\n", branch_num);
	printf("Number of Memory Access Instruction :\t%d\n", mem_num);
    printf("======================================================================\n");
	return;
}
