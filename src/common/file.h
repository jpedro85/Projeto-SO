#ifndef FILE_H
#define FILE_H

#include "cjson/cJSON.h"

cJSON *loadAndParseJSONFromFile(char *fileName);

int loadItemNumber(cJSON* object,char* name,int* number);
int loadItemString(cJSON* object,char* name,char** string);

#endif