#include "calculator.h"

void init(void){
    opcode      = 0x000000;
    pc          = 0;
    op_types    = 0xFFF;
    pflag       = 0b00000000000; 
    op1         = NULL;
    op2         = NULL;
    op3         = NULL;
}
