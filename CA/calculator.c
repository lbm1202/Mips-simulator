#include "calculator.h"

int main(int argc, char* argv[]){
    /*
    if(argc < 2){
        printf("Error : Input file is not exist\n");
        exit(0);
    }
    load(argv[1]);
    */

    load("input.txt");
    init();
    while(InstMemory[pc].opcode){
        printf("PC : %d\t\t| ",pc+1); 

        fetch();
        decode();
        execute();
        
        print_register();
    }

    
    
    return 0;
}