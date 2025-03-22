#include "../header/register.h"

void Reg_file_operation(Register* reg, unsigned int* mux_out){
    reg->reg_file.read_data1 = reg->gp_reg[ *(reg->reg_file.read_reg1) ];
    reg->reg_file.read_data2 = reg->gp_reg[ *(reg->reg_file.read_reg2) ];
    reg->reg_file.write_reg = mux_out;
}

