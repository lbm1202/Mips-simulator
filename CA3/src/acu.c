#include "../header/acu.h"

void ACU_operation(ALU_CU* acu){
    *(acu->jr) = 0;
    switch(*(acu->alu_op)){
		case 0: acu->alu_control = alu_add; break;
		case 1: acu->alu_control = alu_sub; break;
		case 2: acu->alu_control = alu_and; break;
		case 3: acu->alu_control = alu_or;  break;
		case 4: acu->alu_control = alu_xor; break;
		case 5: acu->alu_control = alu_slt; break;
		case 6:
			switch(*(acu->funct)){
				case ADD: 
                case ADDU:  acu->alu_control = alu_add;                     break;
                case JR:    acu->alu_control = alu_add; *(acu->jr) = 1;     break;
                case JALR:  acu->alu_control = alu_add; *(acu->jal) = 1;    break;
				case SUB: 
                case SUBU:  acu->alu_control = alu_sub;                     break;
				case AND:   acu->alu_control = alu_and;                     break;
				case OR:    acu->alu_control = alu_or;                      break;
				case XOR:   acu->alu_control = alu_xor;                     break;
				case NOR:   acu->alu_control = alu_nor;                     break;
				case SLT: 
                case SLTU:  acu->alu_control = alu_slt;                     break;
				case SLL:   acu->alu_control = alu_sll;                     break;
				case SRL:   acu->alu_control = alu_srl;                     break;
				case SRA:   acu->alu_control = alu_sra;                     break;
				case SLLV:  acu->alu_control = alu_sllv;                    break;
				case SRLV:  acu->alu_control = alu_srlv;                    break;
				case SRAV:  acu->alu_control = alu_srav;                    break;
			} break;
        case 7: /*j , jal*/ break;
		default:
            break;
			//error
	}
}