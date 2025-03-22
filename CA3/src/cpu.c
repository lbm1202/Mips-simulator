#include "../header/cpu.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ------------------ single
void fetch(CPU* cpu, Memory* mem){
    if(cpu->reg.PC != 0xFFFFFFFF){
        cpu->IFID[0].valid = 1;
        cpu->IFID[0].data = fetch_ins_mem(mem, cpu->reg.PC);

        cpu->acu.alu_control = 0;

        switch (cpu->control_hazard_option){
            case 0: {cpu->reg.PC = cpu->reg.PC + 4; 
                    cpu->IFID[0].next_PC = cpu->reg.PC;}        break; //detect and wait
            case 1: ANT_prediction(cpu);                        break; //ant
            case 2: ALT_prediction(cpu);                        break; //alt
            case 3: LTP_prediction(cpu);                        break; //dynamic
            default:
                break;
        }
    }
}

void decode(CPU* cpu){
    if (cpu->IFID[1].valid){
        R_type tmp_ins_R = *((R_type*)&(cpu->IFID[1].data));
        I_type tmp_ins_I = *((I_type*)&(cpu->IFID[1].data));
        J_type tmp_ins_J = *((J_type*)&(cpu->IFID[1].data));
        cpu->reg.IR.opcode = tmp_ins_R.opcode;
        cpu->reg.IR.rs = tmp_ins_R.rs;
        cpu->reg.IR.rt = tmp_ins_R.rt;
        cpu->reg.IR.rd = tmp_ins_R.rd;
        cpu->reg.IR.shamt = tmp_ins_R.shamt;
        cpu->reg.IR.funct = tmp_ins_R.funct;
        cpu->reg.IR.imm = tmp_ins_I.imm;
        cpu->reg.IR.addr = tmp_ins_J.addr;

        CU_operation( &(cpu->IDEX[0].control) , cpu->reg.IR.opcode);
        
        Reg_file_operation(  &(cpu->reg) , (cpu->write_reg_mux[ cpu->IDEX[0].control.reg_dst ]) ); 
        cpu->IDEX[0].read_data1 = cpu->reg.reg_file.read_data1; 
        cpu->IDEX[0].read_data2 = cpu->reg.reg_file.read_data2;
        
        cpu->IDEX[0].valid = 1;
        
        //connect unit
        cpu->IDEX[0].opcode = cpu->reg.IR.opcode;
        cpu->IDEX[0].rs = cpu->reg.IR.rs;
        cpu->IDEX[0].rt = cpu->reg.IR.rt;
        cpu->IDEX[0].rd = *(cpu->reg.reg_file.write_reg);
        cpu->IDEX[0].shamt = cpu->reg.IR.shamt;
        cpu->IDEX[0].funct = cpu->reg.IR.funct;
        cpu->IDEX[0].imm = cpu->reg.IR.imm;
        cpu->IDEX[0].next_PC = cpu->IFID[1].next_PC;
        cpu->IDEX[0].addr = cpu->reg.IR.addr;
        cpu->IDEX[0].sign_extened_data = cpu->reg.IR.imm & 0x8000 ? cpu->reg.IR.imm | 0xffff0000 : cpu->reg.IR.imm & 0x0000ffff;
        cpu->IDEX[0].zero_extened_data = cpu->reg.IR.imm & 0x0000ffff;
        cpu->IDEX[0].lui_shifted_data = cpu->reg.IR.imm<<16;

        cpu->IDEX[0].BH_found = cpu->IFID[1].BH_found;
        cpu->IDEX[0].BH_index = cpu->IFID[1].BH_index;
        cpu->IDEX[0].pretaken = cpu->IFID[1].pretaken;

        //j, jal
        cpu->jump_mux[0] = cpu->reg.PC;
        cpu->jump_mux[1] = ((cpu->IFID[1].next_PC)&0xf0000000) | (cpu->IDEX[0].addr << 2);
        cpu->reg.PC = cpu->jump_mux[ cpu->IDEX[0].control.jump ];
    }  
}

void execute(CPU* cpu){
    if (cpu->IDEX[1].valid) {
        cpu->EXMEM[0].control = cpu->IDEX[1].control;
        ACU_operation( &(cpu->acu) );
        int fwd_data = *(cpu->fwd_mtr[ cpu->MEMWB[1].control.mem_to_reg ]);
        unsigned int read_mux1[3] = {   cpu->IDEX[1].read_data1,
                                        fwd_data, 
                                        cpu->EXMEM[1].alu_result };
        unsigned int read_mux2[3] = {   cpu->IDEX[1].read_data2,
                                        fwd_data,
                                        cpu->EXMEM[1].alu_result };
                        
        cpu->EXMEM[0].read_data1 = read_mux1[ cpu->foward_unit[0] ];
        cpu->EXMEM[0].read_data2 = read_mux2[ cpu->foward_unit[1] ];

        ALU_operation( &(cpu->alu) , cpu->alu_src_mux[ cpu->IDEX[1].control.alu_src ]);
        cpu->EXMEM[0].alu_result = cpu->alu.result;
        cpu->EXMEM[0].valid = 1; 

        //connect
        cpu->EXMEM[0].next_PC = cpu->IDEX[1].next_PC;
        cpu->EXMEM[0].flag = cpu->alu.alu_flag; 
        cpu->EXMEM[0].branch_target = cpu->IDEX[1].next_PC + (cpu->IDEX[1].sign_extened_data << 2);
        cpu->EXMEM[0].rd = cpu->IDEX[1].rd;
        
        //branch
        cpu->branch_mux[0] = cpu->EXMEM[0].flag.zero;
        cpu->branch_mux[1] = !cpu->EXMEM[0].flag.zero;
        cpu->branch_mux[2] = cpu->EXMEM[0].flag.lez;
        cpu->branch_mux[3] = cpu->EXMEM[0].flag.gtz;
        unsigned int branch_mux_out = cpu->EXMEM[0].control.branch ? cpu->branch_mux[cpu->EXMEM[0].control.extra & 0x3] : 0; //pcsrc

        cpu->branch_taken_mux[0] = cpu->reg.PC;
        cpu->branch_taken_mux[1] = cpu->EXMEM[0].branch_target;
        unsigned int branch_taken_mux_out = cpu->branch_taken_mux[ branch_mux_out ];    
        
        switch (cpu->control_hazard_option){
            case 0: detect_wait_execution(cpu,branch_mux_out,branch_taken_mux_out); break; //detect and wait;
            case 1: ANT_execution(cpu,branch_mux_out,branch_taken_mux_out);         break; //ant
            case 2: ALT_execution(cpu,branch_mux_out,branch_taken_mux_out);         break; //alt
            case 3: LTP_execution(cpu,branch_mux_out,branch_taken_mux_out);         break; //dynamic
        default:
            break;
        }
        
        //jr jump
        cpu->jump_reg_mux[0] = cpu->reg.PC;
        cpu->jump_reg_mux[1] = cpu->EXMEM[0].alu_result;
        cpu->reg.PC = cpu->jump_reg_mux[ cpu->IDEX[1].control.jr ];
    }
}

void memory_operation(CPU* cpu, Memory* mem){
    if(cpu->EXMEM[1].valid){
        cpu->MEMWB[0].control = cpu->EXMEM[1].control;
        if(cpu->EXMEM[1].control.mem_write){
            store_data_mem(mem, cpu->EXMEM[1].alu_result, cpu->EXMEM[1].read_data2, cpu->EXMEM[1].control.extra);
        }
            
        if(cpu->EXMEM[1].control.mem_to_reg){
            unsigned int lw = fetch_data_mem(mem, cpu->EXMEM[1].alu_result);
            unsigned int lh = (((cpu->EXMEM[1].control.extra)>>2)&1) ? ((int)lw >> 16) : (lw >> 16);
            unsigned int lb = (((cpu->EXMEM[1].control.extra)>>2)&1) ? ((int)lw >> 24) : (lw >> 24);

            cpu->load_mux[0] = lb;
            cpu->load_mux[1] = lh;
            cpu->load_mux[2] = 0;
            cpu->load_mux[3] = lw;

            cpu->MEMWB[0].read_data = cpu->load_mux[ (cpu->EXMEM[1].control.extra & 0x3) ];
        }
        
        cpu->MEMWB[0].valid = 1;
        cpu->MEMWB[0].rd = cpu->EXMEM[1].rd;
        cpu->MEMWB[0].next_PC = cpu->EXMEM[1].next_PC;
        cpu->MEMWB[0].alu_result = cpu->EXMEM[1].alu_result;
    }
}

void writeback(CPU* cpu, Memory* mem){
    if(cpu->MEMWB[1].valid){
        cpu->alu_result_mux[0] = cpu->MEMWB[1].alu_result;
        cpu->alu_result_mux[1] = cpu->MEMWB[1].read_data;
        
        cpu->pc_mem_mux[0] = cpu->alu_result_mux[ cpu->MEMWB[1].control.mem_to_reg ];
        cpu->pc_mem_mux[1] = cpu->MEMWB[1].next_PC + 4;

        
        Reg_file_write(&(cpu->reg), cpu->MEMWB[1].rd , cpu->pc_mem_mux[ cpu->MEMWB[1].control.jal ], cpu->MEMWB[1].control.reg_write);
        
    }
}

void H_detect(CPU* cpu){
    cpu->foward_unit[0] = 0b00;
	cpu->foward_unit[1] = 0b00;

	// EXMEM hazard
	if (cpu->EXMEM[0].rd != 0 && cpu->EXMEM[0].control.reg_write) {
        if(cpu->data_hazard_option == 0){ //wait
            if((cpu->EXMEM[0].rd == cpu->IDEX[0].rs) || (cpu->EXMEM[0].rd == cpu->IDEX[0].rt)){
                cpu->IFID[0].valid = 0;
                cpu->IFID[1].valid = 0;
                cpu->IDEX[0].valid = 0;
                cpu->reg.PC = cpu->EXMEM[0].next_PC;
            }
        }
        else if(cpu->data_hazard_option == 1){ //foward
            if (cpu->EXMEM[0].rd == cpu->IDEX[0].rs) {
                cpu->foward_unit[0] = 0b10;
            }
            if (cpu->EXMEM[0].rd == cpu->IDEX[0].rt) {
                cpu->foward_unit[1] = 0b10;
            }
        }
	}

	// MEMWB hazard
	if (cpu->MEMWB[0].rd != 0 && cpu->MEMWB[0].control.reg_write) {
        if(cpu->data_hazard_option == 0){ //wait
            if ((cpu->EXMEM[0].rd != cpu->IDEX[0].rt && cpu->MEMWB[0].rd == cpu->IDEX[0].rt) || (cpu->EXMEM[0].rd != cpu->IDEX[0].rs && cpu->MEMWB[0].rd == cpu->IDEX[0].rs)){  
                //stall 1 but fail
                cpu->IFID[0].valid = 0;
                cpu->IFID[1].valid = 0;
                cpu->IDEX[0].valid = 0;
                cpu->reg.PC = cpu->EXMEM[0].next_PC;
            }
        }
        else if(cpu->data_hazard_option == 1){ //foward
            if (cpu->EXMEM[0].rd != cpu->IDEX[0].rs && cpu->MEMWB[0].rd == cpu->IDEX[0].rs) {
                cpu->foward_unit[0] = 0b01;
            }
            if (cpu->EXMEM[0].rd != cpu->IDEX[0].rt && cpu->MEMWB[0].rd == cpu->IDEX[0].rt) {
                cpu->foward_unit[1] = 0b01;
            }
        }
	}
}


void latch_update(CPU* cpu){
    memcpy(&(cpu->IFID[1]), &(cpu->IFID[0]), sizeof(IFID_latch));
	memcpy(&(cpu->IDEX[1]), &(cpu->IDEX[0]), sizeof(IDEX_latch));
	memcpy(&(cpu->EXMEM[1]), &(cpu->EXMEM[0]), sizeof(EXMEM_latch));
	memcpy(&(cpu->MEMWB[1]), &(cpu->MEMWB[0]), sizeof(MEMWB_latch));

	memset(&(cpu->IFID[0]), 0, sizeof(IFID_latch));
	memset(&(cpu->IDEX[0]), 0, sizeof(IDEX_latch));
	memset(&(cpu->EXMEM[0]), 0, sizeof(EXMEM_latch));
	memset(&(cpu->MEMWB[0]), 0, sizeof(MEMWB_latch));
}

void init_cpu(CPU* cpu){
    cpu->reg.PC = 0;
    for(int i = 0; i < 32; i++) cpu->reg.gp_reg[i] = 0;
    cpu->reg.gp_reg[29] = 0x01000000;
    cpu->reg.gp_reg[31] = 0xffffffff;

    memset(&(cpu->IFID[0]), 0, sizeof(IFID_latch));
	memset(&(cpu->IDEX[0]), 0, sizeof(IDEX_latch));
	memset(&(cpu->EXMEM[0]), 0, sizeof(EXMEM_latch));
	memset(&(cpu->MEMWB[0]), 0, sizeof(MEMWB_latch));
    memset(&(cpu->IFID[1]), 0, sizeof(IFID_latch));
	memset(&(cpu->IDEX[1]), 0, sizeof(IDEX_latch));
	memset(&(cpu->EXMEM[1]), 0, sizeof(EXMEM_latch));
	memset(&(cpu->MEMWB[1]), 0, sizeof(MEMWB_latch));
    cpu->IFID[0].valid = 0;
	cpu->IDEX[0].valid = 0;
	cpu->EXMEM[0].valid = 0;
	cpu->MEMWB[0].valid = 0;

    memset(&cpu->BH_unit, -1, sizeof(BHU));

    for (int i = 0; i < 0x100; i++) 
        cpu->BH_unit[i].prediction_bit = 0;

    cpu->foward_unit[0] = 0;
    cpu->foward_unit[1] = 0;

    cpu->fwd_mtr[0] = &(cpu->MEMWB[1].alu_result);
    cpu->fwd_mtr[1] = &(cpu->MEMWB[1].read_data);

    cpu->reg.reg_file.read_reg1 = &(cpu->reg.IR.rs);
    cpu->reg.reg_file.read_reg2 = &(cpu->reg.IR.rt);

    cpu->acu.alu_op = &(cpu->IDEX[1].control.alu_op);
    cpu->acu.funct = &(cpu->IDEX[1].funct);
    cpu->acu.jr = &(cpu->IDEX[1].control.jr);
    cpu->acu.jal = &(cpu->IDEX[1].control.jal);

    cpu->alu.alu_control = &(cpu->acu.alu_control);
    cpu->alu.shamt = &(cpu->IDEX[1].shamt);
    cpu->alu.input1 = &(cpu->EXMEM[0].read_data1);

    unsigned int* tmpra = (unsigned int*)malloc(4); *tmpra = 31; //ra
    cpu->write_reg_mux[0] = &(cpu->reg.IR.rt);
    cpu->write_reg_mux[1] = &(cpu->reg.IR.rd); 
    cpu->write_reg_mux[2] = tmpra;
    
    cpu->alu_src_mux[0] = &(cpu->EXMEM[0].read_data2);
    cpu->alu_src_mux[1] = &(cpu->IDEX[1].sign_extened_data); 
    cpu->alu_src_mux[2] = &(cpu->IDEX[1].zero_extened_data);
    cpu->alu_src_mux[3] = &(cpu->IDEX[1].lui_shifted_data);
}