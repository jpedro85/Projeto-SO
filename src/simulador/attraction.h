#ifndef ATTRACTION_H
#define ATTRACTION_H

#include <stdbool.h>

#include "../linked_list.h"

typedef struct Attraction{

    char* name ;
    int minAge, maxAge ;

    LinkedList schedule;

    int duration_ms;
    int rideCapacity;
    
    bool isOpen;
    bool isRunning;

    LinkedList currentAttendance;
    LinkedList waitingLine;

}Attraction;



#endif