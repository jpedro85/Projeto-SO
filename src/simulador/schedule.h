#ifndef SCHEDULE_H
#define SCHEDULE_H

#include "../common/linked_list.h"
typedef struct {
    int startTime_ms;
    int endTime_ms;
}Schedule;

typedef void (*CallFunction)(void*);
typedef struct{
    CallFunction openFunction;
    CallFunction closeFunction;
    LinkedList* scheduleList;
    void* paramsToFunctions;
}ScheduleParams;

void scheduleOpenCloseActiveWait2(LinkedList* scheduleList, CallFunction openFunction, CallFunction closeFunction, void* paramsToFunctions);
void* scheduleOpenCloseActiveWait( void* param );

void scheduleOpenCloseBlocking2(LinkedList* scheduleList, CallFunction openFunction, CallFunction closeFunction, void* paramsToFunctions);
void* scheduleOpenCloseBlocking( void* param );

#endif