#ifndef FILE_H
#define FILE_H

#include "cjson/cJSON.h"

cJSON *loadAndParseJSONFromFile(char *fileName);

#endif