#include "calculator.h"

void trim(char* origin){
    int str_len = strlen(origin);
    int i,j;
    char* tmp;
    char* str = (char*)malloc((str_len+1));
    strcpy(str, origin);
    for(i = 0; origin[i] == ' '; i++){}
    for(j = str_len-1; (origin[j] == ' ')||origin[j] == '\n'; j--){}
    tmp = str+i;
    str[j+1]='\0';
    strcpy(origin, tmp);
    free(str);
}

int hex_to_dec(char* str){
    int sum = 0;
    for(int i = 0; i < strlen(str); i++){
        if(str[i]=='\0')
            break;
        if( str[i] >= 'A' && str[i] <= 'F' )                
            sum = sum * 16 + str[i] - 'A' + 10;
        else if ( str[i] >= 'a' && str[i] <= 'f' )              
            sum = sum * 16 + str[i] - 'a' + 10;
        else if ( str[i] >= '0' && str[i] <= '9' )          
            sum = sum * 16 + str[i] - '0';      
        else{
            printf("Error : invaild value\n");
            exit(0);
        }
    }

    return sum;
}

