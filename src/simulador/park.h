#ifndef PARK_H
#define PARK_H

#include <stdbool.h>

#include "../linked_list.h"

typedef struct Park{

    char* name;
    int parkCapacity;

    LinkedList scheduleList;
    LinkedList attractions;

}Park;

#endif