#include "calculator.h"
#define BUF_SIZE 100

void load(char* program){
    FILE* fp;
    char buffer[BUF_SIZE];
    char* ptr, * tmp;
    int count, opcode, memno, num;

    fp = fopen(program, "r");
    if (fp == NULL) {
        printf("Error : program is not exist\n");
        exit(0);
    }


    memno = 0;
    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        count = 0;
        trim(buffer);
        ptr = strtok(buffer, " ");                  //string tokenize at first token
        InstMemory[memno].operand_types = 0xFFF;  
        while(ptr != NULL){
            tmp = (char*)malloc(BUF_SIZE);
            strcpy(tmp,ptr);                    
            switch (count){
                case 0: InstMemory[memno].opcode = STRING_TO_OPCODE(tmp); break;
                case 1: /* in case 1,2,3(operand), if operand is register, then bind register pointer */
                case 2: /* else if operand is hex number, then bind malloc func pointer and put value ,else NULL */ 
                case 3: if( tmp[0] == 'r' ){
                            if((num = hex_to_dec(tmp+1)) < 10){
                                InstMemory[memno].operand[count-1] = reg+num;
                                InstMemory[memno].operand_types &= (num << (4 * (3-count))) | ~(0xF<<(4 * (3-count)));
                            }
                            else{
                                InstMemory[memno].operand[count-1] = NULL;
                                InstMemory[memno].operand_types &= (0xE << (4 * (3-count))) | ~(0xF<<(4 * (3-count)));
                            }
                        }
                        else if((tmp[0]=='0') && (tmp[1]=='x')){
                            InstMemory[memno].operand[count-1] = (uint32_t*)malloc(sizeof(int));
                            *(InstMemory[memno].operand[count-1]) = hex_to_dec(tmp+2);
                            InstMemory[memno].operand_types &= (0xA << (4 * (3-count))) | ~(0xF<<(4 * (3-count)));
                        }
                        else if((*((unsigned long*)(tmp))) == STDOUT){
                            InstMemory[memno].operand_types &= (0xB << (4 * (3-count))) | ~(0xF<<(4 * (3-count)));
                        }
                        else{
                            InstMemory[memno].operand_types &= (0xE << (4 * (3-count))) | ~(0xF<<(4 * (3-count)));
                        }
                default:
                    break;
            }
            ptr = strtok(NULL, " ");  //next token
            free(tmp);
            count++;
        }
        if(integrity_check(InstMemory[memno])){
            printf("Invaild Instruction! \n");
            exit(0);
        }
        memno++;
    }
    fclose(fp);
}
