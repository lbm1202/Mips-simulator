#ifndef _MEMORY_H
#define _MEMORY_H

typedef struct _Memory{
    char* mem;
    unsigned int text_area_start;
    unsigned int text_area_end;
    unsigned int data_area_start;
    unsigned int data_area_end;
}Memory;

void init_memory(Memory* memory,unsigned int size,int text_area_start,int text_area_end,int data_area_start,int data_area_end);
int load_program(Memory* memory,char* dir);
int load_word(unsigned int address, char* mem);
unsigned int fetch_ins_mem(Memory* memory,unsigned int address);
unsigned int fetch_data_mem(Memory* memory,unsigned int address);

void store_data_mem(Memory* memory,unsigned int address,unsigned int value,unsigned int count);

#endif