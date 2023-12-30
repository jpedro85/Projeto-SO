#ifndef PARK_H
#define PARK_H

#include <semaphore.h>
#include <stdbool.h>
#include <pthread.h>
#include "../common/linked_list.h"

typedef struct {

    char* name;
    int parkCapacity;
    bool isOpen;
    pthread_rwlock_t parkIsOpen_rwlock_t;

    LinkedList scheduleList;
    LinkedList attractions;

    sem_t parkVacancy_sem_t;

}Park;

void startPark();
void startParkSimulation();

#endif