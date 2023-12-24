#ifndef GLOBALS_H
#define GLOBALS_H

#include "park.h"
#include "simulationConf.h"

//#include <semaphore.h>
#include <time.h>

typedef enum{
    STARTING,
    RUNNING,
    PAUSED,
    ENDED
}SimulationStatusEnum;

extern int simulationStatus;
extern pthread_rwlock_t simulationStatus_rwlock_t;

extern Park park;
extern SimulationConf simulationConf;
extern struct timespec startTime;

#endif