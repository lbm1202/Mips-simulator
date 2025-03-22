#include "header/cpu.h"
#include "header/memory.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MEMSIZE 0xFFFFFFFF

void set_option(CPU* cpu, char* argv[]);
void print_cycle(CPU* cpu);
void print_fetch(CPU* cpu, int tmp_pc);
void print_decode(CPU* cpu);
void print_execute(CPU* cpu);
void print_mem_op(CPU* cpu);
void print_writeback(CPU* cpu);
void print_fowarding(CPU* cpu);
void print_result(CPU* cpu);


int cycle = 0, R_num = 0, I_num = 0, J_num = 0, branch_num = 0, mem_num = 0, stall_num = 0;


int main(int argc, char* argv[]){
    int tmp_pc = 0;
	if(argc < 4){
        printf("Error : Input file is not exist\n ./main (input file) (data hazard option) (control hazard option)\n");
		printf("data hazard option\n\tdetect and wait : 0\n\tfowarding : 1\ncontrol hazard option\n\tdetect and wait : 0\n\tANT : 1\n\tALT : 2\n\tLTP : 3\n");
        exit(0);
    }

	
	
	//argv[3];

    //char dir[100] = "input/input1/input.bin";
    //printf("choose input file(1~5) : "); scanf("%c",dir+11);
	CPU cpu;
	Memory memory;

	//printf("data_hazard option(0 : detect and wait, 1 : fowarding) : "); scanf("%d",&(cpu.data_hazard_option));
	//printf("control_hazard option(0 : detect and wait, 1 : ANT, 2 : ALT, 3 : LTP) : "); scanf("%d",&(cpu.control_hazard_option));
    

    init_cpu(&cpu);
    init_memory(&memory, MEMSIZE , 0x0,0x00100000, 0x00100001,0x01001000);

	set_option(&cpu, argv);


    if(load_program(&memory, argv[1])) return 0;

    while( cpu.reg.PC != 0xFFFFFFFF){

		
		memory_operation(&cpu, &memory);
		writeback(&cpu, &memory);				print_cycle(&cpu); tmp_pc = cpu.reg.PC;
        fetch(&cpu, &memory);					print_fetch(&cpu,tmp_pc);
        decode(&cpu);							print_decode(&cpu);
        execute(&cpu);							print_execute(&cpu); print_mem_op(&cpu); print_writeback(&cpu); 

		H_detect(&cpu); 						print_fowarding(&cpu);
		latch_update(&cpu);
    }
    print_result(&cpu);
    
    return 0;
    
}

void set_option(CPU* cpu, char* argv[]){
	if(!strcmp((argv[2]), "0")){
		cpu->data_hazard_option = 0;
	}
	else if(!strcmp((argv[2]), "1")){
		cpu->data_hazard_option = 1;
	}
	else{
		cpu->data_hazard_option = 1;
	}

	if(!strcmp((argv[3]), "0")){
		cpu->control_hazard_option = 0;
	}
	else if(!strcmp((argv[3]), "1")){
		cpu->control_hazard_option = 1;
	}
	else if(!strcmp((argv[3]), "2")){
		cpu->control_hazard_option = 2;
	}
	else if(!strcmp((argv[3]), "3")){
		cpu->control_hazard_option = 3;
	}
	else{
		cpu->control_hazard_option = 1;
	}
}

void print_cycle(CPU* cpu){
	printf("\nCycle[%d] (PC: 0x%x)\n", ++cycle, cpu->reg.PC);
}

void print_fetch(CPU* cpu, int tmp_pc){
    printf(" [IF]\n");
	printf("\t0x%08x\t pc: 0x%x\n", cpu->IFID[0].data, tmp_pc);
	printf("\t[PC update] pc <- 0x%x\n",cpu->IFID[0].next_PC);
}

void print_decode(CPU* cpu){
	printf(" [ID] \n");
	if (cpu->IDEX[0].valid){
		if (cpu->IFID[1].data == 0){
			printf("\tNop\n");
		}
		else if (cpu->IDEX[0].opcode == R){
			printf("\ttype: R, ");
			printf("opcode: 0x%x, ", cpu->IDEX[0].opcode);
			printf("rs: 0x%x (R[%d]=0x%x), ", cpu->IDEX[0].rs, cpu->IDEX[0].rs, cpu->reg.gp_reg[cpu->IDEX[0].rs]);
			printf("rt: 0x%x (R[%d]=0x%x), ", cpu->IDEX[0].rt, cpu->IDEX[0].rt, cpu->reg.gp_reg[cpu->IDEX[0].rt]);
			printf("rd: 0x%x (%d), ", cpu->IDEX[0].rd, cpu->IDEX[0].rd);
			printf("shamt: 0x%x, ", cpu->IDEX[0].shamt);
			printf("funct: 0x%x\n", cpu->IDEX[0].funct);
			R_num++;
		}
		else if (cpu->IDEX[0].opcode == J || cpu->IDEX[0].opcode == JAL){
			printf("\ttype: J, ");
			printf("opcode: 0x%x, ", cpu->IDEX[0].opcode);
			printf("address: 0x%x\n", cpu->IDEX[0].addr);
			J_num++;
		}
		else{
			printf("\ttype: I, ");
			printf("opcode: 0x%x, ", cpu->IDEX[0].opcode);
			printf("rs: 0x%x (R[%d]=0x%x), ", cpu->IDEX[0].rs, cpu->IDEX[0].rs, cpu->reg.gp_reg[cpu->IDEX[0].rs]);
			printf("rt: 0x%x (R[%d]=0x%x), ", cpu->IDEX[0].rt, cpu->IDEX[0].rt, cpu->reg.gp_reg[cpu->IDEX[0].rt]);
			printf("imm: 0x%x\n", cpu->IDEX[0].imm);
			I_num++;
		}

		if(cpu->IDEX[0].control.jump){
			printf("\t[PC update (jump)] pc <- 0x%x\n",cpu->reg.PC);
		}
	}
	else{
		printf("\t[ Stall ]\n");
		stall_num++;
	}
}

void print_execute(CPU* cpu){
	printf(" [EXE] \n");
	if (cpu->EXMEM[0].valid){
		if(cpu->IDEX[1].control.alu_src == 0){
			printf("\trs: %x(r[%d]=%x), rt: %x(r[%d]=%x), alu result : %x\n",cpu->IDEX[1].rs,cpu->IDEX[1].rs,cpu->EXMEM[0].read_data1, cpu->IDEX[1].rt,cpu->IDEX[1].rt,cpu->EXMEM[0].read_data2, cpu->EXMEM[0].alu_result);
		}
		else{
			printf("\trs: %x(r[%d]=%x), imm: %x, alu result : %x\n",cpu->IDEX[1].rs,cpu->IDEX[1].rs,cpu->EXMEM[0].read_data1, cpu->EXMEM[0].read_data2, cpu->EXMEM[0].alu_result);
		}

		if(cpu->IDEX[1].control.jr){
			printf("\t[PC update (jump register)] pc <- 0x%x\n",cpu->reg.PC);
		}
		if(cpu->IDEX[1].control.branch){
			printf("\tbranch target : 0x%08x\n",cpu->EXMEM[0].branch_target);
		}
		if(cpu->EXMEM[0].control.branch ? cpu->branch_mux[cpu->EXMEM[0].control.extra & 0x3] : 0){
			printf("\t[PC update (branch)] pc <- 0x%x\n",cpu->reg.PC);
			branch_num++;
		}
	}
	else{
		printf("\t[ STALL ]\n");
		stall_num++;
	}
}

void print_mem_op(CPU* cpu){
	printf(" [MEM] \n");
	if(cpu->EXMEM[1].valid){
		if(cpu->EXMEM[1].control.mem_write){
			printf("\t[STORE] M[0x%08x]  <- %x\n",cpu->EXMEM[1].alu_result, cpu->EXMEM[1].read_data2);
			mem_num++;
		}
		if(cpu->EXMEM[1].control.mem_to_reg){
			printf("\t[MEMORY ACCESS] read_data <- M[0x%08x] = %x\n",cpu->EXMEM[1].alu_result, cpu->MEMWB[0].read_data);
			mem_num++;
		}
	}
	else{
		printf("\t[ STALL ]\n");
		stall_num++;
	}
}

void print_writeback(CPU* cpu){
	printf(" [WB] \n");
    if(cpu->MEMWB[1].valid){
		if(cpu->MEMWB[1].control.reg_write){
			printf("\tr[%d] <- %x\n",cpu->MEMWB[1].rd, cpu->pc_mem_mux[ cpu->MEMWB[1].control.jal ]);
		}
		
	}
	else{
		printf("\t[ STALL ]\n");
		stall_num++;
	}
}

void print_fowarding(CPU* cpu){
	printf(" [Fowarding] \n");
	if(cpu->foward_unit[0] == 0b10){
			printf("\trs<-[EXMEM Fowarding]\n");
		}
		else if (cpu->foward_unit[0] == 0b01){
			printf("\trs<-[MEMWB Fowarding]\n");
		}

		if(cpu->foward_unit[1] == 0b10){
			printf("\trt<-[EXMEM Fowarding]\n");
		}
		else if (cpu->foward_unit[1] == 0b01){
			printf("\trt<-[MEMWB Fowarding]\n");
		}
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
	printf("Number of STALL :\t\t\t%d\n", stall_num);
    printf("======================================================================\n");
	return;
}
