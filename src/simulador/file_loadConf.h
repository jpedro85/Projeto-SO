#ifndef FILE_FILECONF_H
#define FILE_FILECONF_H

#include "park.h"
#include "simulationConf.h"

void loadConfig(Park* park,SimulationConf* simulationConf, char* configFile);

int verifyLoadedValues(Park* park,SimulationConf* simulationConf);

#endif