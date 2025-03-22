#include "calculator.h"

inst InstMemory[0x1000000];
uint32_t  opcode, pc;
uint16_t op_types, pflag;
uint32_t* op1, * op2, * op3;
inst IR;
uint32_t reg[10];

void decode(void){
    opcode = IR.opcode;
    op1 = IR.operand[0];
    op2 = IR.operand[1];
    op3 = IR.operand[2];
    op_types =IR.operand_types;
}
