#ifndef _REGISTER_H
#define _REGISTER_H

typedef struct _register_file{
    unsigned int* read_reg1;
    unsigned int* read_reg2;
    unsigned int* write_reg;
    unsigned int write_data;

    
    unsigned int read_data1;
    unsigned int read_data2;


    unsigned int* reg_write;
}Register_file;

typedef struct _Instruction_register{
    unsigned int data;
    unsigned int opcode;
    unsigned int rs;
    unsigned int rt;
    unsigned int rd;
    unsigned int shamt;
    unsigned int funct;
    unsigned int imm;
    unsigned int addr;
}Ins_reg;

typedef struct _reg{
    unsigned int PC;
    Ins_reg IR;
    unsigned int gp_reg[32];
    Register_file reg_file;
}Register;


void Reg_file_operation(Register* reg, unsigned int* mux_out);

#endif