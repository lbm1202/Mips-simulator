#ifndef _ACU_H
#define _ACU_H

#define SLL		0x00
#define SRL		0x02
#define SRA		0x03
#define SLLV	0x04
#define SRLV	0x06
#define SRAV	0x07
#define JR		0x08
#define JALR	0x09
#define MOVZ	0x0a
#define MOVN	0x0b
#define ADD		0x20
#define ADDU	0x21
#define SUB		0x22
#define SUBU	0x23
#define AND		0x24
#define OR		0x25
#define XOR		0x26
#define NOR 	0x27
#define SLT		0x2a
#define SLTU	0x2b


#define alu_and		0x00
#define alu_or		0x01
#define alu_add 	0x02
#define alu_sll		0x04
#define alu_xor		0x05
#define alu_nor 	0x06
#define alu_srl 	0x07
#define alu_sra		0x08
#define alu_sub		0x12
#define alu_slt		0x13
#define alu_sllv	0x24
#define alu_srlv 	0x27
#define alu_srav	0x28

typedef struct _ALU_CU{
    unsigned int* alu_op;
    unsigned int* funct;
    unsigned int* jal;
    unsigned int* jr;
    unsigned int alu_control;
}ALU_CU;

void ACU_operation(ALU_CU* acu);

#endif