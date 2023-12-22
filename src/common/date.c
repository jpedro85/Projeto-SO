#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "string_utils.h"
#include "date.h"
#include "consoleAddons.h"
#define NANO_S_TO_SECONDS 1000000000.0

/**
 * The function getCurrentSimulationDate calculates the current date and time based on the elapsed time since the simulation began.
 * 
 * @param beginTime The beginTime parameter is a struct timespec that represents the starting time of
 * the simulation.
 * @param currentTime The currentTime parameter is a struct timespec variable that represents the
 * current.
 * @param dayLength_s The parameter "dayLength_s" represents the length of a day in seconds. 
 * @return a variable of type Date.
 */
Date getCurrentSimulationDate(struct timespec beginTime, int dayLength_s ){

    struct timespec currentTime;
    clock_gettime(CLOCK_REALTIME,&currentTime);

    const long beginTime_ns = (beginTime.tv_sec * NANO_S_TO_SECONDS) + beginTime.tv_nsec;
    const long currentTime_ns = (currentTime.tv_sec * NANO_S_TO_SECONDS) + currentTime.tv_nsec;
    const double hour_s = (double)dayLength_s / 24;
    const double min_s = hour_s / 60;

    Date date;
    double timeDeference_s = (currentTime_ns - beginTime_ns) / NANO_S_TO_SECONDS;

    date.day = timeDeference_s / dayLength_s;
    double remainingTime_s = timeDeference_s - date.day * dayLength_s;
    
    double hourOfDay_aux = remainingTime_s / hour_s;
    date.hour =  hourOfDay_aux > 24 ? 24 : hourOfDay_aux ;
    double remainingTimeHour_s = remainingTime_s - date.hour * hour_s;

    int minute = remainingTimeHour_s / min_s;
    date.minute = (minute > 60 ) ? 60 : minute;
    
    return date;
}

/**
 * The function "dateToString" converts a Date object into a string representation.
 * 
 * @param date The "date" parameter is of type "Date".
 * 
 * @return a pointer to a character array (string) in the format "d: [day] h: [hour] m: [minute]".
 */
char* dateToString(Date date){

    char* strDate;
    asprintf(&strDate,"d:%d h:%d m:%d",date.day,date.hour,date.minute);
    return strDate;
}