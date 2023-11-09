#ifndef MUTEX_ADDONS_H
#define MUTEX_ADDONS_H

#include <pthread.h>

void lockMutex(pthread_mutex_t* mutex , char* name);
void unlockMutex(pthread_mutex_t* mutex , char* name);

#endif