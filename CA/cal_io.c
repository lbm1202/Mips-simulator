#include "calculator.h"


int get_reg_index(int op_types){
    return 0b1<<(( op_types>> (8)) & 0xF);
}

void print_register(void){
    if((pflag>>10) == 1){
        pflag = pflag & 0b01111111111;
    }
    else{
        printf("\t\t|");
    }

    if(pflag){ 
        for(int i = 0; i < 10; i++){
            if(((pflag>>i) & 0b1)==0b1)
                printf(" reg[%d]=%d",i,reg[i]);
        }
        
    }
    printf("\n");
}
