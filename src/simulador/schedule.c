#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#include "schedule.h"
#include "globals.h"
#include "../common/linked_list.h"
#include "../common/mutexAddons.h"
#include "../common/consoleAddons.h"
#include "../common/date.h"


/**
 * The function activeWait waits for a specified amount of time in milliseconds using an active waiting
 * approach relative to (global)startTime;
 * 
 * @param time_ms The parameter "time_ms" represents the desired amount of time to wait in
 * milliseconds.
 */
int activeWait(int time_ms ,int day){

    // int elapsedTime = 0;
    // do{
    //     //activeWait
    //     elapsedTime = getCurrentTime_ms(startTime) % simulationConf.dayLength_ms ;
    // }while( elapsedTime < time_ms );

    // printf("elapsedTime %d", elapsedTime);
    // return elapsedTime;

    int elapsedTime = -1;
    int elapsedTime_mod = -1;
    int actualDay = -1;
    do{
        //activeWait
        elapsedTime = getCurrentTime_ms(startTime);

        elapsedTime_mod = elapsedTime % simulationConf.dayLength_ms;
        actualDay = elapsedTime / simulationConf.dayLength_ms;

    }while( elapsedTime_mod < time_ms && day == actualDay);

    printf("elapsedTime %d", elapsedTime_mod);
    return elapsedTime_mod;
}


/**
 * The function "activeWaitEndOfDay" waits until the specified day has ended in a simulation.
 * 
 * @param day The "day" parameter represents the specific day that the function is waiting for.
 */
void activeWaitEndOfDay(int day){

   
    int elapsedTime = 0;
    int actualDay = -1;

    do{
       
        elapsedTime = getCurrentTime_ms(startTime);
        actualDay = (elapsedTime / simulationConf.dayLength_ms);

    }while( actualDay == day);
    
    printf("EndOFFDay\n");
}
/**
 * The function "activeWaitForTime" waits until a specified time has passed and then calls a given
 * function.
 * 
 * @param time_ms The time in milliseconds that the function should wait for before calling the
 * provided function.
 * @param function The "function" parameter is a function pointer that represents a function to be
 * called after the specified time has passed.
 * 
 * @return If the condition `time_ms >= simulationConf.dayLength_s * 1000` is true, then the function
 * will return without executing any further code.
 */
void activeWaitForTime(int time_ms, int day, CallFunction function, void* paramsToFunctions){    

    printOption("entered activeWaitForTime");
    activeWait(time_ms,day);
    function(paramsToFunctions);
}

/**
 * calls "scheduleOpenCloseActiveWait".
 * 
 * @param scheduleList A pointer to a LinkedList object that contains the schedule information.
 * @param openFunction A function pointer to the function that will be called to open something. This
 * function should have a void return type and no parameters.
 * @param closeFunction The closeFunction parameter is a function pointer that points to a function
 * that will be called to close something.
 */
void scheduleOpenCloseActiveWait2(LinkedList* scheduleList, CallFunction openFunction, CallFunction closeFunction, void* paramsToFunctions){

    ScheduleParams param;
    param.openFunction = openFunction;
    param.closeFunction = closeFunction;
    param.paramsToFunctions  = paramsToFunctions;
    scheduleOpenCloseActiveWait(&param);
}

/**
 * The function schedules and executes a series of open and close functions based on a list of
 * schedules.
 * 
 * @param param The `param` parameter is a void pointer that is cast to a `ScheduleParams` pointer
 * inside the function. It is used to pass additional parameters to the function.
 */
void* scheduleOpenCloseActiveWait( void* param ){

    printOption("entered scheduleOpenCloseActiveWait");

    ScheduleParams* scheduleParams = (ScheduleParams*)param;
    Schedule actualSchedule;
    int actualScheduleIndex = 0;
    int currentDay = 0;

    int simStatus = -1;
    do{
        readlock(&simulationStatus_rwlock_t,"simulationStatus_rwlock_t");
        simStatus = simulationStatus;
        rwlock_unlock(&simulationStatus_rwlock_t,"simulationStatus_rwlock_t");
    }while(simStatus != RUNNING);

    do{ 
        //loop foreach day
        actualScheduleIndex = 0;

        readlock(&simulationStatus_rwlock_t,"simulationStatus_rwlock_t");
        simStatus = simulationStatus;
        rwlock_unlock(&simulationStatus_rwlock_t,"simulationStatus_rwlock_t");

        if ( currentDay > simulationConf.numberOfDaysToSimulate || simStatus == ENDED) 
            break; 

       
        printf("%d",currentDay);
        printOption("scheduleOpenCloseActiveWait NewDay");

        if ( currentDay == simulationConf.numberOfDaysToSimulate )
            break; 
        
        for (actualScheduleIndex; actualScheduleIndex < scheduleParams->scheduleList->length ; actualScheduleIndex++)
        {
            actualSchedule = *((Schedule*)getValueByIndex_LInkedList(scheduleParams->scheduleList,actualScheduleIndex));
            activeWaitForTime(actualSchedule.startTime_ms, currentDay, scheduleParams->openFunction,scheduleParams->paramsToFunctions);
            activeWaitForTime(actualSchedule.endTime_ms, currentDay, scheduleParams->closeFunction,scheduleParams->paramsToFunctions);
        }

        activeWaitEndOfDay(currentDay);

        currentDay++;
  
    }while(true);

    free(param);

    printOption("ended scheduleOpenCloseActiveWait");

    pthread_exit(NULL);
}

/**
 * calls "scheduleOpenCloseBlocking"
 * 
 * @param scheduleList A pointer to a LinkedList object that contains the schedule information.
 * @param openFunction A function pointer to the function that will be called to open something.
 * @param closeFunction The closeFunction parameter is a function pointer that points to a function
 * that will be called to close something.
 */
void scheduleOpenCloseBlocking2(LinkedList* scheduleList, CallFunction openFunction, CallFunction closeFunction, void* paramsToFunctions){

    ScheduleParams param;
    param.openFunction = openFunction;
    param.closeFunction = closeFunction;
    param.paramsToFunctions = paramsToFunctions;
    scheduleOpenCloseBlocking(&param);
}

/**
 * The function `scheduleOpenCloseBlocking` is a thread function that executes a schedule of opening
 * and closing actions based on a given schedule list.
 * 
 * @param param The `param` parameter is a void pointer that can be used to pass any type of data to
 * the `scheduleOpenCloseBlocking` function. In this case, it is casted to a `ScheduleParams*` type,
 * which suggests that it is expected to be a pointer to a `Schedule
 */
void* scheduleOpenCloseBlocking( void* param ){

    ScheduleParams* scheduleParams = (ScheduleParams*)param;
    Schedule actualSchedule;
    int actualScheduleIndex = 0;
    int lastTime = 0;

    int waitTimeToOpen_ms = 0;
    int waitTimeToClose_ms = 0;  
    int currentDay = 1;

    int simStatus = -1;
    do{
        readlock(&simulationStatus_rwlock_t,"simulationStatus_rwlock_t");
        simStatus = simulationStatus;
        rwlock_unlock(&simulationStatus_rwlock_t,"simulationStatus_rwlock_t");
    }while(simStatus != RUNNING);

    do{ 
        //loop foreach day
        actualScheduleIndex = 0;

        readlock(&simulationStatus_rwlock_t,"simulationStatus_rwlock_t");
        simStatus = simulationStatus;
        rwlock_unlock(&simulationStatus_rwlock_t,"simulationStatus_rwlock_t");

        if ( currentDay > simulationConf.numberOfDaysToSimulate || simStatus == ENDED) 
            break; 

        printf("%d",currentDay);
        printOption("scheduleOpenCloseActiveWait NewDay");
        
        lastTime = 0;
        for (actualScheduleIndex; actualScheduleIndex < scheduleParams->scheduleList->length ; actualScheduleIndex++)
        {
            actualSchedule = *((Schedule*)getValueByIndex_LInkedList(scheduleParams->scheduleList,actualScheduleIndex));

            waitTimeToOpen_ms = actualSchedule.startTime_ms - lastTime;
            waitTimeToClose_ms = actualSchedule.endTime_ms - actualSchedule.startTime_ms;

            if(waitTimeToOpen_ms != 0){
                usleep(waitTimeToOpen_ms * 1000 );
            }
            scheduleParams->openFunction(scheduleParams->paramsToFunctions);//open
            usleep(waitTimeToClose_ms * 1000);
            scheduleParams->closeFunction(scheduleParams->paramsToFunctions);//close

            lastTime = actualSchedule.endTime_ms;
        }

        activeWaitEndOfDay(currentDay);
        currentDay++;


    }while(true);

    free(param);

}