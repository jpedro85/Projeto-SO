#ifndef ATTRACTION_H
#define ATTRACTION_H

#include <stdbool.h>

#include "../common/linked_list.h"

typedef struct {

    char* name ;
    int minAge, maxAge ;

    LinkedList scheduleList;

    int duration_ms;
    int rideCapacity;
    
    bool isOpen;
    bool isRunning;

    sem_t currentAttendance;
    LinkedList waitingLine;

}Attraction;

void initializeAttraction(Attraction *attraction);
void enterAttraction(User *client, Attraction *attraction);
void leaveAttraction(User *client, Attraction *attraction);
#endif