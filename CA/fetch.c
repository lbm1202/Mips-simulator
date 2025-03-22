#include "calculator.h"

void fetch(void){
    IR = InstMemory[pc];
    pc = pc + 1;
}
