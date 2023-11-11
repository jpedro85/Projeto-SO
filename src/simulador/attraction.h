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

    LinkedList currentAttendance;
    LinkedList waitingLine;

}Attraction;

#endif