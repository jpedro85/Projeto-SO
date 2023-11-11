#ifndef DATE_H
#define DATE_H

typedef struct Date{
    int day;
    int hour;
    int minute;
}Date;

Date simulationDateNow();

#endif