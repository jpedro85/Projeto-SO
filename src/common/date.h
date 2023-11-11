#ifndef DATE_H
#define DATE_H
#include <time.h>
typedef struct Date{
    int day;
    int hour;
    int minute;
}Date;

Date getCurrentSimulationDate(struct timespec beginTime, struct timespec currentTime, int dayLength_s );

#endif