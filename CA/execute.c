#include "calculator.h"

void execute(void){
    switch (opcode){
        case ADD: *op1 = *op2 + *op3;                           pflag = pflag | get_reg_index(op_types); break; //
        case SUB: *op1 = *op2 - *op3;                           pflag = pflag | get_reg_index(op_types); break; //
        case MUL: *op1 = *op2 * *op3;                           pflag = pflag | get_reg_index(op_types); break; //
        case DIV: *op1 = *op2 / *op3;                           pflag = pflag | get_reg_index(op_types); break; //
        case SLT: *op1 = *op2 < *op3 ? 1 : 0;                   pflag = pflag | get_reg_index(op_types); break; //
        case MOV: *op1 = *op2;                                  pflag = pflag | get_reg_index(op_types); break; //
        case  LW: *op1 = *op2;                                  pflag = pflag | get_reg_index(op_types); break; //
        case  SW: printf("STDOUT : %d\t|",*op1);                pflag = pflag | 0b10000000000; break;
        case RST: for(int i = 0; i < 10; i++) reg[i] = 0;       pflag = 0b00000000000; break;
        case JMP: pc = *op1 - 1; break;
        case BEQ: pc = *op1 == *op2 ? *op3 - 1 : pc; break;
        case BNE: pc = *op1 != *op2 ? *op3 - 1 : pc; break;
        
        default:
            break;
    }
}