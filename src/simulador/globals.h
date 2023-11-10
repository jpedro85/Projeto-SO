#ifndef GLOBALS_H
#define GLOBALS_H

#include "park.h"
#include "simulationConf.h"

#include <semaphore.h>

extern Park park;
extern SimulationConf simulationConf;
extern sem_t parkVacancy;

#endif