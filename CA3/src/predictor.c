#include "../header/cpu.h"

void detect_wait_execution(CPU* cpu, unsigned int branch_taken, unsigned int addr){
    if(cpu->IDEX[1].control.branch){
        cpu->IFID[0] = cpu->IFID[1];
        cpu->IDEX[0].valid = 0;
        
        if(branch_taken){
            cpu->reg.PC = addr;
        }
        else{
            cpu->reg.PC = cpu->IFID[1].next_PC-4;
        }
    }
}

void ANT_prediction(CPU* cpu){
    cpu->reg.PC = cpu->reg.PC + 4; 
    cpu->IFID[0].next_PC = cpu->reg.PC;
}

void ANT_execution(CPU* cpu, unsigned int branch_taken, unsigned int addr){
    if (branch_taken) { 

		cpu->IFID[0].valid = 0;
		cpu->IFID[1].valid = 0;
		cpu->IDEX[0].valid = 0;
	}
	else {
		//pass
	}
    cpu->reg.PC = addr;
}


void ALT_prediction(CPU* cpu){
    int BH_index = (0x100 - 1) ;
    BH_index <<= 2;
    BH_index &= cpu->reg.PC;
    BH_index >>= 2;
	cpu->IFID[0].BH_index = BH_index;

	if (cpu->reg.PC == cpu->BH_unit[BH_index].branch_inst_addr) {
		cpu->IFID[0].BH_found = 1;
		cpu->IFID[0].next_PC = cpu->reg.PC + 4;

		cpu->reg.PC = cpu->BH_unit[BH_index].branch_target_addr;
	}
	else {
		cpu->IFID[0].BH_found = 0;
		cpu->reg.PC = cpu->reg.PC + 4;
		cpu->IFID[0].next_PC = cpu->reg.PC;
	}
}

void ALT_execution(CPU* cpu, unsigned int branch_taken, unsigned int addr){
	if (cpu->IDEX[1].BH_found==1) {
		if (branch_taken) {
			//pass
		}
		else {
			cpu->reg.PC = cpu->IDEX[1].next_PC;

			cpu->IFID[0].valid = 0;
			cpu->IFID[1].valid = 0;
			cpu->IDEX[0].valid = 0;
		}
	}
	else {
		if (branch_taken) {
			cpu->BH_unit[cpu->IDEX[1].BH_index].branch_inst_addr = cpu->IDEX[1].next_PC - 4; 
			cpu->reg.PC = addr;
			cpu->BH_unit[cpu->IDEX[1].BH_index].branch_target_addr = cpu->reg.PC; 
			
			cpu->IFID[0].valid = 0;
			cpu->IFID[1].valid = 0;
			cpu->IDEX[0].valid = 0;
		}
	}
}

void LTP_prediction(CPU* cpu){
	int BH_index = (0x100 - 1) ;
    BH_index <<= 2;
    BH_index &= cpu->reg.PC;
    BH_index >>= 2;
	cpu->IFID[0].BH_index = BH_index;
	
	//0: not taken
	//1: taken 
	if (cpu->reg.PC == cpu->BH_unit[BH_index].branch_inst_addr) {
		cpu->IFID[0].BH_found = 1;
		switch (cpu->BH_unit[BH_index].prediction_bit){
		case 0:     cpu->IFID[0].pretaken = 0;
			        cpu->reg.PC = cpu->reg.PC + 4;
			        cpu->IFID[0].next_PC = cpu->reg.PC;                     break;
		
        case 1:     cpu->IFID[0].pretaken = 1;
			        cpu->IFID[0].next_PC = cpu->reg.PC + 4;
			        cpu->reg.PC = cpu->BH_unit[BH_index].branch_target_addr;  break;
		}
	}
	else {
        cpu->reg.PC = cpu->reg.PC + 4;
		cpu->IFID[0].BH_found = 0;
		cpu->IFID[0].pretaken = 0;
		cpu->IFID[0].next_PC = cpu->reg.PC;
	}
}

void LTP_execution(CPU* cpu, unsigned int branch_taken, unsigned int addr){
    //0: not taken
    //1: taken
	if (cpu->IDEX[1].BH_found) { //found BH_unit in IF stage
		if (cpu->IDEX[1].pretaken) { //predicted taken 
			if (branch_taken) {
				cpu->BH_unit[cpu->IDEX[1].BH_index].prediction_bit = 1;
			}
			else {
				cpu->BH_unit[cpu->IDEX[1].BH_index].prediction_bit = 0;
				cpu->reg.PC = cpu->IDEX[1].next_PC;

				cpu->IFID[0].valid = 0;
				cpu->IFID[1].valid = 0;
				cpu->IDEX[0].valid = 0;
			}
		}
		else { //predicted not taken
			if (branch_taken) {
				cpu->BH_unit[cpu->IDEX[1].BH_index].prediction_bit = 1;
				cpu->reg.PC = cpu->BH_unit[cpu->IDEX[1].BH_index].branch_target_addr;

				cpu->IFID[0].valid = 0;
				cpu->IFID[1].valid = 0;
				cpu->IDEX[0].valid = 0;
			}
			else {
				cpu->BH_unit[cpu->IDEX[1].BH_index].prediction_bit = 0;
			}
		}
	}
	else { // not found BH_unit in IF stage
		if (branch_taken) { 
			cpu->BH_unit[cpu->IDEX[1].BH_index].branch_inst_addr = cpu->IDEX[1].next_PC - 4;
			cpu->reg.PC = addr;
			cpu->BH_unit[cpu->IDEX[1].BH_index].branch_target_addr = cpu->reg.PC; 
			cpu->BH_unit[cpu->IDEX[1].BH_index].prediction_bit = 1; 

			cpu->IFID[0].valid = 0;
			cpu->IFID[1].valid = 0;
			cpu->IDEX[0].valid = 0;
		}
	}
}
 