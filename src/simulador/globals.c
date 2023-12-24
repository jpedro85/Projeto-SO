#include "globals.h"

Park park;
SimulationConf simulationConf;
struct timespec startTime;
struct timespec currentDayStartTime;

int simulationStatus;
pthread_rwlock_t simulationStatus_rwlock_t;

