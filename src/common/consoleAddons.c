#include <stdio.h>
#include <stdlib.h>

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

void printWarning(char* warning){
    printf("\033[1;33m%s\033[1;0m\n",warning); 
}

void printFatalError(char* error){
    printf("\033[1;31m%s\033[1;0m\n",error); 
    printWarning("The program will exit!");
    exit(1);
}

void printSuccess(char* success){
    printf("\033[1;32m%s\033[1;0m\n",success); 
}

void resetColor(){
    printf("\033[1;0m");
}