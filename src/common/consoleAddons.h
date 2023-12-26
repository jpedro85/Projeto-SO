#ifndef CONSOLEADDONS_H
#define CONSOLEADDONS_H

//printf wit colors
void printResult(char* result);
void printOption(char* option);
void printInfo(char* info);
void printError(char* error);
void printFatalError(char* error);
void printWarning(char* warning);
void printWithColor(int r ,int g ,int b , char* msg);
void printSuccess(char* success);
void printCursor();
void resetColor();

#endif