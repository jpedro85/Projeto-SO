#ifndef SCHEDULE_H
#define SCHEDULE_H

#include <stdbool.h>

typedef struct {
    int begin;
    int end;
    bool done;
}Schedule;

#endif