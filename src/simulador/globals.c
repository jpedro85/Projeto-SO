#include "globals.h"

Park park;
SimulationConf simulationConf;
struct timespec startTime;

int simulationStatus;
pthread_rwlock_t simulationStatus_rwlock_t;
