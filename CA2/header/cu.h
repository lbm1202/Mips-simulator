#ifndef _CU_H
#define _CU_H

#define R 		0x00
#define BLTZ	0x01
#define J		0x02
#define JAL		0x03
#define BEQ		0x04
#define BNE		0x05
#define BLEZ	0x06
#define BGTZ	0x07
#define ADDI	0x08
#define ADDIU	0x09
#define ALTI	0x0a
#define ALTIU	0x0b
#define ANDI	0x0c
#define ORI		0x0d
#define XORI	0x0e
#define LUI		0x0f
	
	
#define LB		0x20
#define LH		0x21
#define LWL		0x22
#define LW		0x23
#define LBU		0x24
#define LHU		0x25
#define LWR		0x26
#define SB		0x28
#define SH		0x29
#define SWL		0x2a
#define SW		0x2b
#define SWR		0x2e

typedef struct _CU{
    unsigned int reg_dst;
    unsigned int reg_write;

    unsigned int jump;
    unsigned int jal;
    unsigned int jr;

    unsigned int alu_src;
    unsigned int alu_op;
    
    unsigned int mem_to_reg;
    unsigned int mem_write;

    unsigned int branch;
    unsigned int extra;
    
    unsigned int* opcode;
}CU;

void CU_operation(CU* cu);

#endif