#ifndef GLOBALS_H
#define GLOBALS_H

#include "park.h"
#include "simulationConf.h"

#include <semaphore.h>
#include <time.h>

extern Park park;
extern SimulationConf simulationConf;
extern struct timespec startTime;

#endif