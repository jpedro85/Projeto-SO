#ifndef PARK_H
#define PARK_H

#include <stdbool.h>

#include "../common/linked_list.h"

typedef struct {

    char* name;
    int parkCapacity;

    LinkedList scheduleList;
    LinkedList attractions;

}Park;

#endif