#ifndef DATE_H
#define DATE_H
#include <time.h>
typedef struct Date{
    int day;
    int hour;
    int minute;
}Date;

Date getCurrentSimulationDate(struct timespec beginTime, int dayLength_s );

char* dateToString(Date date);

int getCurrentTime_ms(struct timespec dayBeginTime);

void getActualTimespec(struct timespec* currentTime);

#endif