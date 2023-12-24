#ifndef ATTRACTION_H
#define ATTRACTION_H

#include <pthread.h>
#include "../common/linked_list.h"
#include <semaphore.h>
#include <stdbool.h>

typedef struct {

    char* name ;
    int minAge, maxAge ;

    LinkedList scheduleList;

    int duration_ms;
    int rideCapacity;
    int attractionRideMinLoad;
    int rideBeginMaxWaitTime_ms;
    
    bool isOpen;
    pthread_rwlock_t isOpen_rwlock_t;

    bool isRunning;
    pthread_rwlock_t isRunning_rwlock_t;

    int currentAttendance;
    sem_t enterRide_sem_t;
    pthread_t rideCycles_thread;

    pthread_mutex_t currentAttendance_mut_t;

    LinkedList waitingLine;

}Attraction;

void startAttractionSimulation();

#endif