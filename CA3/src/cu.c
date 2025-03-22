#include "../header/cu.h"

void CU_operation(CU* cu, int opcode){
    //reg_dst
    cu->reg_dst = (opcode == R) ? 1 : (opcode == JAL) ? 2 : 0 ;

    //reg_write
    cu->reg_write = ((opcode == R) || (opcode == JAL)) || ((opcode & 0x38) == 0x20) || ((opcode & 0x38) == 0x08);

    //jump
    cu->jump = ((opcode == J) || (opcode == JAL));

    //jal
    cu->jal = (opcode == JAL);

    //mem_write
    cu->mem_write = ((opcode & 0x38) == 0x28) ? 1 : 0 ; //sw,sh,sb

	//mem_to_reg
    cu->mem_to_reg = ((opcode & 0x38) == 0x20) ? 1 : 0 ; //lw ,lb(u), lh(u)

    //branch
    cu->branch = (opcode & 0x3c) == 0x4 ;

    //extra
    cu->extra = (opcode & 0x07);


    //alu_src
    if ( ((opcode & 0x3c)== 0x4 ) || (opcode == R) ){ //branch or R
        cu->alu_src = 0; //read data2 == read register2
    }
    if (((opcode & 0x3c) == 0x8) || ((opcode & 0x30) == 0x20)){ //load, store , arithmatic
		cu->alu_src = 1; //read data2 == imm with sign ext
	}
    else if ((opcode & 0x3c) == 0xc){ //
		cu->alu_src = 2; //read data2 == imm with zero ext
	}
    else if (opcode == LUI){ //LUI
		cu->alu_src = 3;    //read data2 == imm with sll 16
	}
	
    //alu_op
    if (((opcode & 0x30)) == 0x20 || ((opcode & 0x3e) == 0x08) || (opcode == LUI)){
		cu->alu_op = 0b000;
	}
	else if ((opcode & 0x3c) == 0x04){ //Branch
		cu->alu_op = 0b001;
	}
	else if (opcode == ANDI){ 
		cu->alu_op = 0b010;
	}
    else if (opcode == ORI){ 
		cu->alu_op = 0b011;
	}
    else if (opcode == XORI){ 
		cu->alu_op = 0b100;
	}
	else if ((opcode & 0x3e) == 0x0a){
		cu->alu_op = 0b101;
	}
	else if (opcode == R){
		cu->alu_op = 0b110;
	}
	else if (opcode == J || opcode == JAL){
        cu->alu_op = 0b111; //spcial alu_op
	}
	else{
		//error
	}
}