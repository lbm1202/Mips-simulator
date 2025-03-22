#include "../header/memory.h"
#include <stdio.h>
#include <stdlib.h>

void init_memory(Memory* memory,unsigned int size,int text_area_start,int text_area_end,int data_area_start,int data_area_end){
    memory->mem = (char*)calloc(size,1);
    memory->text_area_start = text_area_start;
    memory->text_area_end = text_area_end;
    memory->data_area_start = data_area_start;
    memory->data_area_end = data_area_end;
}

int load_program(Memory* memory,char* dir){
    FILE* file = fopen(dir, "r");
    if(file==NULL){
        printf("[ERROR] input file error\n");
        return 1;
    }
    int i = 0;

	while(fgets(memory->mem+i, 5, file) != NULL) 
        i += 4;
    return 0;
}

int load_word(unsigned int address, char* mem){
	int ret = 0;
	for(int i = 0; i<4; i++){
		ret = ret<<8;
		ret = ret | (mem[address+i]&0xff);
	}
	return ret;
}	

unsigned int fetch_ins_mem(Memory* memory,unsigned int addr){
    if((memory->text_area_end >= addr)  &&  (addr >= memory->text_area_start ))
        return load_word(addr, memory->mem);
    else
        return 0;
}

unsigned int fetch_data_mem(Memory* memory,unsigned int addr){
    if((memory->data_area_end >= addr)  &&  (addr >= memory->data_area_start ))
        return load_word(addr, memory->mem);
    else
        return 0;
}

void store_data_mem(Memory* memory,unsigned int addr,unsigned int value,unsigned int count){
    if((memory->data_area_end >= addr+count)  &&  (addr >= memory->data_area_start )){
        int tmp = value << 8 * (3 - count);
        for(int i = 0; i <= count; i++){
            memory->mem[addr + i] = (tmp >> 24) & 0xff;
            tmp = tmp << 8;
        }
    }
    else
        return;
}

