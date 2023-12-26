#ifndef FILE_H
#define FILE_H

#include "cjson/cJSON.h"

cJSON *loadAndParseJSONFromFile(char *fileName);

int loadItemNumber2(cJSON* object,char* name,int* number);
int loadItemNumber(cJSON* object,char* name,int* number);
int loadItemString2(cJSON* object,char* name,char** string);
int loadItemString(cJSON* object,char* name,char** string);

#endif