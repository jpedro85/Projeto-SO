#ifndef ATTRACTION_H
#define ATTRACTION_H

#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>

#include "../common/linked_list.h"
#include "user.h"

typedef struct {

    char* name ;
    int minAge, maxAge ;

    LinkedList scheduleList;

    int duration_ms;
    int rideCapacity;
    int attractionRideMinLoad;
    int rideBeginMaxWaitTime_ms;
    int rideCounter;
    
    bool isOpen;
    pthread_rwlock_t isOpen_rwlock_t;

    bool isRunning;
    pthread_rwlock_t isRunning_rwlock_t;

    int currentAttendance;
    sem_t enterRide_sem_t;
    pthread_t rideCycles_thread;

    pthread_mutex_t currentAttendance_mut_t;

    //User LinkedList
    LinkedList waitingLine;
    pthread_mutex_t waitingLine_mutex_t;

}Attraction;

void startAttractionSimulation();

void enterAttraction(User *client, Attraction *attraction);
void leaveAttraction(User *client, Attraction *attraction);

#endif