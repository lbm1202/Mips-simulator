#include "../header/register.h"
#include <stdio.h>

void Reg_file_operation(Register* reg, unsigned int* mux_out){
    reg->reg_file.read_data1 = reg->gp_reg[ *(reg->reg_file.read_reg1) ];
    reg->reg_file.read_data2 = reg->gp_reg[ *(reg->reg_file.read_reg2) ];
    reg->reg_file.write_reg = mux_out;
}

void Reg_file_write(Register* reg, unsigned int write_reg, unsigned int  write_data, unsigned int enable){
    if ((write_reg > 0) && (enable==1)) {
        reg->gp_reg[write_reg] = write_data;
    }
}