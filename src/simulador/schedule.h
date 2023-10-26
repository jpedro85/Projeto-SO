#ifndef SCHEDULE_H
#define SCHEDULE_H

#include <stdbool.h>

typedef struct {
    int startTime_ms;
    int endTime_ms;
    bool done;
}Schedule;

#endif