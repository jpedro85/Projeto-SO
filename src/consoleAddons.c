#include <stdio.h>

void printResult(char* result){
    printf("\033[1;33m%s\033[1;0m\n",result);
}

void printOption(char* option){
    printf("\033[1;36m%s\033[1;0m\n",option);
}

void printInfo(char* info){
    printf("\033[1;37m%s\033[1;0m\n",info);
}

void printCursor(){
    printf("\033[1;35m->\033[1;32m");
}

void printError(char* error){
    printf("\033[1;31m%s\033[1;0m\n",error); 
}

void resetColor(){
    printf("\033[1;0m");
}