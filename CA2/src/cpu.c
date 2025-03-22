#include "../header/cpu.h"
#include <stdio.h>
#include <stdlib.h>

// ------------------ single
void init_cpu(CPU* cpu){
    cpu->reg.PC = 0;
    for(int i = 0; i < 32; i++) cpu->reg.gp_reg[i] = 0;
    cpu->reg.gp_reg[29] = 0x01000000;
    cpu->reg.gp_reg[31] = 0xffffffff;
    cpu->reg.reg_file.reg_write = &(cpu->cu.reg_write);

    cpu->reg.reg_file.read_reg1 = &(cpu->reg.IR.rs);
    cpu->reg.reg_file.read_reg2 = &(cpu->reg.IR.rt);

    cpu->cu.opcode = &(cpu->reg.IR.opcode);

    cpu->acu.alu_op = &(cpu->cu.alu_op);
    cpu->acu.funct = &(cpu->reg.IR.funct);
    cpu->acu.jr = &(cpu->cu.jr);
    cpu->acu.jal = &(cpu->cu.jal);

    unsigned int* tmpra = (unsigned int*)malloc(4); *tmpra = 31; //ra
    cpu->write_reg_mux[0] = &(cpu->reg.IR.rt);
    cpu->write_reg_mux[1] = &(cpu->reg.IR.rd); 
    cpu->write_reg_mux[2] = tmpra;
    
    cpu->alu_src_mux[0] = &(cpu->reg.reg_file.read_data2);
    cpu->alu_src_mux[1] = &(cpu->sign_extened_data);
    cpu->alu_src_mux[2] = &(cpu->zero_extened_data);
    cpu->alu_src_mux[3] = &(cpu->lui_shifted_data);

    cpu->alu.alu_control = &(cpu->acu.alu_control);
    cpu->alu.input1 = &(cpu->reg.reg_file.read_data1);
    cpu->alu.shamt = &(cpu->reg.IR.shamt);
}

void fetch(CPU* cpu, Memory* mem){
    cpu->reg.IR.data = fetch_ins_mem(mem, cpu->reg.PC);

    cpu->pc_adder = cpu->reg.PC + 4;
}

void decode(CPU* cpu){
    R_type tmp_ins_R = *((R_type*)&(cpu->reg.IR.data));
    I_type tmp_ins_I = *((I_type*)&(cpu->reg.IR.data));
    J_type tmp_ins_J = *((J_type*)&(cpu->reg.IR.data));

    cpu->reg.IR.opcode = tmp_ins_R.opcode;
    cpu->reg.IR.rs = tmp_ins_R.rs;
    cpu->reg.IR.rt = tmp_ins_R.rt;
    cpu->reg.IR.rd = tmp_ins_R.rd;
    cpu->reg.IR.shamt = tmp_ins_R.shamt;
    cpu->reg.IR.funct = tmp_ins_R.funct;
    cpu->reg.IR.imm = tmp_ins_I.imm;
    cpu->reg.IR.addr = tmp_ins_J.addr;
    
    CU_operation( &(cpu->cu) );
    ACU_operation( &(cpu->acu) );
    Reg_file_operation(  &(cpu->reg) , (cpu->write_reg_mux[ cpu->cu.reg_dst ]) );

    cpu->sign_extened_data = cpu->reg.IR.imm & 0x8000 ? cpu->reg.IR.imm | 0xffff0000 : cpu->reg.IR.imm & 0x0000ffff;
    cpu->zero_extened_data = cpu->reg.IR.imm & 0x0000ffff;
    cpu->lui_shifted_data = cpu->reg.IR.imm<<16;

    cpu->branch_adder = cpu->pc_adder + (cpu->sign_extened_data << 2);
}

void execute(CPU* cpu){
    ALU_operation( &(cpu->alu) , cpu->alu_src_mux[ cpu->cu.alu_src ]);
}

void memory_operation(CPU* cpu, Memory* mem){
    if(cpu->reg.IR.data == 0) 
        return;
    if(cpu->cu.mem_write)
        store_data_mem(mem, cpu->alu.result, cpu->reg.reg_file.read_data2, cpu->cu.extra);
}

void writeback(CPU* cpu, Memory* mem){
    if(cpu->reg.IR.data == 0){
        PC_operation(cpu);
        return;
    }
    
    unsigned int lw = fetch_data_mem(mem, cpu->alu.result);
    unsigned int lh = (((cpu->cu.extra)>>2)&1) ? ((int)lw >> 16) : (lw >> 16);
    unsigned int lb = (((cpu->cu.extra)>>2)&1) ? ((int)lw >> 24) : (lw >> 24);

    cpu->load_mux[0] = lb;
    cpu->load_mux[1] = lh;
    cpu->load_mux[2] = 0;
    cpu->load_mux[3] = lw;

    unsigned int load_mux_out = cpu->load_mux[ (cpu->cu.extra & 0x3) ];

    cpu->alu_result_mux[0] = cpu->alu.result;
    cpu->alu_result_mux[1] = load_mux_out;

    unsigned int alu_result_mux_out = cpu->alu_result_mux[ cpu->cu.mem_to_reg ];

    //reg_write
    cpu->jal_adder = cpu->pc_adder + 4;

    cpu->pc_mem_mux[0] = alu_result_mux_out;
    cpu->pc_mem_mux[1] = cpu->jal_adder;

    unsigned int pc_mem_mux_out = cpu->pc_mem_mux[ cpu->cu.jal ];
    cpu->reg.reg_file.write_data = pc_mem_mux_out;

    if( *(cpu->reg.reg_file.reg_write) && *(cpu->reg.reg_file.write_reg)){
         cpu->reg.gp_reg[ *(cpu->reg.reg_file.write_reg) ] = cpu->reg.reg_file.write_data; //memory write
    }
    
    PC_operation(cpu);
}

void PC_operation(CPU* cpu){
    //------pc update
    
    cpu->branch_mux[0] = cpu->alu.alu_flag.zero;
    cpu->branch_mux[1] = !cpu->alu.alu_flag.zero;
    cpu->branch_mux[2] = cpu->alu.alu_flag.lez;
    cpu->branch_mux[3] = cpu->alu.alu_flag.gtz;

    
    unsigned int branch_mux_out = cpu->cu.branch ? cpu->branch_mux[cpu->cu.extra & 0x3] : 0; //pcsrc

    cpu->branch_taken_mux[0] = cpu->pc_adder;
    cpu->branch_taken_mux[1] = cpu->branch_adder;
    unsigned int branch_taken_mux_out = cpu->branch_taken_mux[ branch_mux_out ];

    cpu->jump_mux[0] = branch_taken_mux_out;
    cpu->jump_mux[1] = ((cpu->pc_adder)&0xf0000000) | (cpu->reg.IR.addr << 2);
    unsigned int jump_mux_out = cpu->jump_mux[ cpu->cu.jump ];

    cpu->jump_reg_mux[0] = jump_mux_out;
    cpu->jump_reg_mux[1] = cpu->alu_result_mux[cpu->cu.mem_to_reg];
    unsigned int jump_reg_mux_out = cpu->jump_reg_mux[ cpu->cu.jr ];
	cpu->reg.PC = jump_reg_mux_out;
}