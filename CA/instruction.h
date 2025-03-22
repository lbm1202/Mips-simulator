#pragma once

//#define STRING_TO_OPCODE(opcode) ((*((unsigned int*)(opcode)))&0xffffff)
#define STRING_TO_OPCODE(opcode) ( opcode[0] | (opcode[1] << 8) | (opcode[2] << 16) )

#define ADD 0x444441
#define SUB 0x425553
#define MUL 0x4c554d
#define DIV 0x564944
#define MOV 0x564f4d
#define LW  0x574c
#define SW  0x5753
#define RST 0x545352
#define JMP 0x504d4a
#define BEQ 0x514542
#define BNE 0x454e42
#define STDOUT 0x54554F445453
#define SLT 0x544c53

typedef struct _instruction{
    uint32_t* operand[3];
    uint32_t opcode : 24;
    uint16_t operand_types : 12;
}inst;

