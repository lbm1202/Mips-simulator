#ifndef _ALU_H
#define _ALU_H

typedef struct _ALU_flag{
    int zero;
    int negative;
    int gtz;
    int lez;
    int carry;
    int overflow;
}ALU_flag;

typedef struct _ALU{
    unsigned int* alu_control;
    unsigned int* input1;
    unsigned int* input2;
    unsigned int* shamt;

    unsigned int input2_mux[2];
    unsigned int shamt_mux[2];
    unsigned int out_mux[9];
    ALU_flag alu_flag;
    unsigned int result;
}ALU;

void ALU_operation(ALU* alu, unsigned int* mux_out);

#endif