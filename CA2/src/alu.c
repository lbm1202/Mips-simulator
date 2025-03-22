#include "../header/alu.h"

void ALU_operation(ALU* alu, unsigned int* mux_out){
    alu->input2 = mux_out;

    alu->input2_mux[0] = *(alu->input2); 
    alu->input2_mux[1] = ~(*(alu->input2));

    int input2_mux_out = alu->input2_mux[ ( *(alu->alu_control) >>4) & 1 ];
    int sum = (*(alu->input1)) + (input2_mux_out) + ((*(alu->alu_control) >>4)&1);
    int slt = (sum>>31)&1;
    
    
    //shift//
    alu->shamt_mux[0] = *(alu->shamt);
    alu->shamt_mux[1] = *(alu->input2);

    int shamt_mux_out = alu->shamt_mux[(*(alu->alu_control) >>5)&1];
    int srl = (*(alu->input1)) >> shamt_mux_out;
    int sll = (*(alu->input1)) << shamt_mux_out;

    int sra = (int)(*(alu->input1)) >> shamt_mux_out;
    //----//

    alu->out_mux[0] = (*(alu->input1)) & input2_mux_out;
    alu->out_mux[1] = (*(alu->input1)) | input2_mux_out;
    alu->out_mux[2] = sum;
    alu->out_mux[3] = slt;
    alu->out_mux[4] = sll;
    alu->out_mux[5] = (*(alu->input1)) ^ input2_mux_out;
    alu->out_mux[6] = ~((*(alu->input1)) | input2_mux_out);
    alu->out_mux[7] = srl;
    alu->out_mux[8] = sra;

    alu->result = alu->out_mux[ *(alu->alu_control) & 0x7 ];

    alu->alu_flag.zero = !(alu->result);
    alu->alu_flag.negative = ((alu->result>>31)&1);
    alu->alu_flag.gtz = !(alu->alu_flag.negative || alu->alu_flag.zero); // result > 0
    alu->alu_flag.lez = alu->alu_flag.zero || alu->alu_flag.negative; // result <= 0
}