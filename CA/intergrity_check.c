#include "calculator.h"

/*
Optypes = 0x ? ? ?
             │ │ └ operand2 types
　　　　　	    │ └ operand1 types
　　　　　		└ operand0 types
operand type
0~9 regiseter = 0~9 hex value
immediate value = 0xA value
STDOUT = 0xB
NONE = 0xF
Error = 0xE
  
ex.
Optypes = 0x 2 0 1
             │ │ └ operand2 types == register[1]
　　　　　		│ └ operand1 types == register[0]
　　　　　		└ operand0 types == register[2]
　　　　　			 
Optypes = 0x 0 A F
             │ │ └ operand2 types == None
　　　　　		│ └ operand1 types == immediate
　　　　　		└ operand0 types == register[0]
　　　　　			 
Optypes = 0x 1 B F
             │ │ └ operand2 types == None
　　　　　		│ └ operand1 types == STDOUT
　　　　　		└ operand0 types == register[1]
*/

int integrity_check(inst ins){
    int op_types = ins.operand_types;
    int op[3] = { (( op_types>> (8)) & 0xF) , (( op_types>> (4)) & 0xF), op_types & 0xF};
    
    switch (ins.opcode){
        // op1: r,  op2: r, op3: r
        case ADD:
        case SUB:
        case MUL:
        case DIV:   for(int i = 0; i < 3; i++)
                        if(op[i] > 0x9)
                            return 1; //invaild value
                    break;
         
        // op1: r/n,  op2: r/n, op3: n
        case BEQ:
        case BNE:   if(op[0] > 0xA || op[1] > 0xA || op[2] != 0xA) return 1; //invaild value
                    break;
        
        //op1: r,  op2: r/n, op3: n
        case SLT:   if(op[0] > 0x9 || op[1] > 0xA || op[2] > 0xA) return 1; //invaild value
                    break;
        //op1: r,  op2: r/n, op3: X
        case MOV:   if(op[0] > 0x9 || op[1] > 0xA || op[2] != 0xF) return 1; //invaild value
                    break;
        //op1: r,  op2: n, op3: X
        case  LW:   if(op[0] > 0x9 || op[1] != 0xA || op[2] != 0xF) return 1; //invaild value
                    break;
        //op1: r,  op2: STDOUT, op3: X
        case  SW:   if(op[0] > 0x9 || op[1] != 0xB || op[2] != 0xF) return 1; //invaild value
                    break;
        //op1: n,  op2: X, op3: X
        case JMP:   if(op[0] != 0xA || op[1] != 0xF || op[2] != 0xF) return 1; //invaild value
                    break;
        //op1: X,  op2: X, op3: X
        case RST:   if(op[0] != 0xF || op[1] != 0xF || op[2] != 0xF) return 1; //invaild value
                    break;

        default:
            break;
    }
    return 0;
}
