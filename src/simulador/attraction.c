#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "globals.h"
#include "../common/consoleAddons.h"
#include "../common/linked_list.h"
#include "../common/date.h"
#include "../common/mutexAddons.h"
#include "../common/events.h"
#include "socketServer/socketServer.h"
#include "attraction.h"
#include "schedule.h"
#include "../common/date.h"


/**
 * The function "beginRide" waits for certain conditions to be met before starting a ride at an
 * attraction.
 * 
 * @param attraction The "attraction" parameter is a pointer to an object of type "Attraction".
 */
void beginRide(Attraction* attraction ){

    struct timespec startRideTime;
    getActualTimespec(&startRideTime);
    //wait start conditions less then max time and min load
    int currentAttendance = 0;
    do{
        //TODO:Maybe a problem here Delay when trying to get lock
        lockMutex(&(attraction->currentAttendance_mut_t),"currentAttendance_mut_t");
        currentAttendance = attraction->currentAttendance;
        unlockMutex(&(attraction->currentAttendance_mut_t),"currentAttendance_mut_t");

        //TODO:currentAttendance > 0 ADD (not added because of tests)
    }while( attraction->isOpen && currentAttendance < attraction->attractionRideMinLoad && getCurrentTime_ms(startRideTime) <= attraction->rideBeginMaxWaitTime_ms);

    //if(attraction->isOpen){
   // }
    printSuccess("ride Began");
    EventInfo_AttractionRideEvent eventInfo;
    eventInfo.attractionName = attraction->name;

    writelock(&(attraction->isRunning_rwlock_t),"isRunning_rwlock_t");
    attraction->isRunning = true;
    attraction->rideCounter++;
    eventInfo.rideNumber = attraction->rideCounter;
    rwlock_unlock(&(attraction->isRunning_rwlock_t),"isRunning_rwlock_t");

    eventInfo.usersInRide = currentAttendance;
    //TODO:Maybe a mutex is needed to check list.length 
    eventInfo.usersWaiting = attraction->waitingLine.length;

    asyncCreateEvent_AttractionRideEvent(getCurrentSimulationDate(startTime,simulationConf.dayLength_s),eventInfo,RIDE_STARTED,5,addMsgToQueue);
}

void endRide(Attraction* attraction ){

    printSuccess("ride Ended");

    EventInfo_AttractionRideEvent eventInfo;
    eventInfo.attractionName = attraction->name;

    //TODO:Maybe a problem here Delay when trying to get lock
    lockMutex(&(attraction->currentAttendance_mut_t),"currentAttendance_mut_t");
    eventInfo.usersInRide = attraction->currentAttendance;
    unlockMutex(&(attraction->currentAttendance_mut_t),"currentAttendance_mut_t");

    //TODO:Maybe a mutex is needed to check list.length 
    eventInfo.usersWaiting = attraction->waitingLine.length;

    writelock(&(attraction->isRunning_rwlock_t),"isRunning_rwlock_t");
    attraction->isRunning = false;
    eventInfo.rideNumber = attraction->rideCounter;
    rwlock_unlock(&(attraction->isRunning_rwlock_t),"isRunning_rwlock_t");

    asyncCreateEvent_AttractionRideEvent(getCurrentSimulationDate(startTime,simulationConf.dayLength_s),eventInfo,RIDE_ENDED,5,addMsgToQueue);
}

void* startRideCycle(void* param ){

    printOption("startRideCycle Created");

    Attraction* attraction = (Attraction*)param;
    if(attraction->duration_ms != 0){

        while(attraction->isOpen){

            pthread_testcancel();
            beginRide(attraction);
            usleep(attraction->duration_ms * 1000);
            pthread_testcancel();
            printOption("AfterSleep");
            endRide(attraction);
        }

        if(attraction->isRunning){
            writelock(&(attraction->isRunning_rwlock_t),"isRunning_rwlock_t");
            attraction->isRunning = false;
            rwlock_unlock(&(attraction->isRunning_rwlock_t),"isRunning_rwlock_t");
        }

        pthread_testcancel();

    }else{

        printSuccess("ride started");

        EventInfo_AttractionRideEvent eventInfo;
        eventInfo.attractionName = attraction->name;

        writelock(&(attraction->isRunning_rwlock_t),"isRunning_rwlock_t");
        attraction->isRunning = true;
        attraction->rideCounter++;
        eventInfo.rideNumber = attraction->rideCounter;
        rwlock_unlock(&(attraction->isRunning_rwlock_t),"isRunning_rwlock_t");

        //TODO:Maybe a problem here Delay when trying to get lock
        lockMutex(&(attraction->currentAttendance_mut_t),"currentAttendance_mut_t");
        eventInfo.usersInRide = attraction->currentAttendance;
        unlockMutex(&(attraction->currentAttendance_mut_t),"currentAttendance_mut_t");

        //TODO:Maybe a mutex is needed to check list.length 
        eventInfo.usersWaiting = attraction->waitingLine.length;
        asyncCreateEvent_AttractionRideEvent(getCurrentSimulationDate(startTime,simulationConf.dayLength_s),eventInfo,RIDE_STARTED,5,addMsgToQueue);
    }

    printOption("startRideCycle exited");
    pthread_exit(NULL);
}

void stopRideCycle(Attraction* attraction ){

    printOption("stopRideCycle called");

    //TODO: in user if duration_ms == 0 and isOpen is false user must leave the attraction
    if(attraction->duration_ms == 0){

        printSuccess("ride ended");

        EventInfo_AttractionRideEvent eventInfo;
        eventInfo.attractionName = attraction->name;

        writelock(&(attraction->isRunning_rwlock_t),"isRunning_rwlock_t");
        attraction->isRunning = false;
        eventInfo.rideNumber = attraction->rideCounter;
        rwlock_unlock(&(attraction->isRunning_rwlock_t),"isRunning_rwlock_t");

        //TODO:Maybe a problem here Delay when trying to get lock
        lockMutex(&(attraction->currentAttendance_mut_t),"currentAttendance_mut_t");
        eventInfo.usersInRide = attraction->currentAttendance;
        unlockMutex(&(attraction->currentAttendance_mut_t),"currentAttendance_mut_t");

        //TODO:Maybe a mutex is needed to check list.length 
        eventInfo.usersWaiting = attraction->waitingLine.length;

        asyncCreateEvent_AttractionRideEvent(getCurrentSimulationDate(startTime,simulationConf.dayLength_s),eventInfo,RIDE_ENDED,5,addMsgToQueue);

    }else{

        int error = pthread_cancel(attraction->rideCycles_thread);
        if(error != 0 && error != ESRCH)
            printFatalError("Can not cancel thread rideCycles_thread");

        if(attraction->isRunning){

            EventInfo_AttractionRideEvent eventInfo;
            eventInfo.attractionName = attraction->name;

            writelock(&(attraction->isRunning_rwlock_t),"isRunning_rwlock_t");
            attraction->isRunning = false;
            eventInfo.rideNumber = attraction->rideCounter;
            rwlock_unlock(&(attraction->isRunning_rwlock_t),"isRunning_rwlock_t");

            //TODO:Maybe a problem here Delay when trying to get lock
            lockMutex(&(attraction->currentAttendance_mut_t),"currentAttendance_mut_t");
            eventInfo.usersInRide = attraction->currentAttendance;
            unlockMutex(&(attraction->currentAttendance_mut_t),"currentAttendance_mut_t");

            //TODO:Maybe a mutex is needed to check list.length 
            eventInfo.usersWaiting = attraction->waitingLine.length;

            asyncCreateEvent_AttractionRideEvent(getCurrentSimulationDate(startTime,simulationConf.dayLength_s),eventInfo,RIDE_ENDED,5,addMsgToQueue);
            
            printSuccess("ride ended2");
        }

    }
}


void openAttraction(void* attractionP){

    printWarning("openAttraction called");

    Attraction* attraction = (Attraction*)attractionP;

    writelock(&(attraction->isOpen_rwlock_t),"isOpen_rwlock_t");
    attraction->isOpen = true;
    rwlock_unlock(&(attraction->isOpen_rwlock_t),"isOpen_rwlock_t");

    attraction->rideCounter = 0;

    EventInfo_AttractionEvent eventInfo;
    eventInfo.attractionName = attraction->name;
    asyncCreateEvent_AttractionEvent(getCurrentSimulationDate(startTime,simulationConf.dayLength_s),eventInfo,ATTRACTION_OPEN,4,addMsgToQueue);
//    // create_DetachThread(startRideCycle,attractionP,"startRideCycle");
    if( pthread_create(&(attraction->rideCycles_thread),NULL,startRideCycle, attractionP) < 0){
        printError(attraction->name);
        printFatalError("Could not creat thread for startRideCycle.");
    }

}

void closeAttraction(void* attractionP){

    printWarning("CloseAttraction called");

    Attraction* attraction = (Attraction*)attractionP;
    writelock(&(attraction->isOpen_rwlock_t),"isOpen_rwlock_t");
    attraction->isOpen = false;
    rwlock_unlock(&(attraction->isOpen_rwlock_t),"isOpen_rwlock_t");
    
    stopRideCycle(attraction);

    EventInfo_AttractionEvent eventInfo;
    eventInfo.attractionName = attraction->name;
    asyncCreateEvent_AttractionEvent(getCurrentSimulationDate(startTime,simulationConf.dayLength_s),eventInfo,ATTRACTION_CLOSED,4,addMsgToQueue);
}

void startAttraction(void* attractionP){

    Attraction* attraction = (Attraction*)attractionP; 
    attraction->isOpen = false;
    //init the lock;
    rwlock_init(&(attraction->isOpen_rwlock_t),"isOpen_rwlock_t");
    rwlock_init(&(attraction->isRunning_rwlock_t),"isRunning_rwlock_t");

    attraction->isRunning = false;
    attraction->currentAttendance = 0;
    mutex_init(&(attraction->currentAttendance_mut_t),"currentAttendance_mut_t");
    semInit(&(attraction->enterRide_sem_t),attraction->rideCapacity,"enterRide_sem_t");
}

/**
 * The function starts the simulation for each attraction in the park by initializing the attraction,
 * creating a thread to handle the opening and closing schedule.
 */
void startAttractionSimulation(){

    ListItem* listItem_attraction;
    ForEach_LinkedList((&(park.attractions)),listItem_attraction)
    {
        //init attraction
        startAttraction(listItem_attraction->value);

        ScheduleParams* params = (ScheduleParams*)malloc(sizeof(ScheduleParams));
        params->openFunction = openAttraction;
        params->closeFunction = closeAttraction;
        params->scheduleList = &(((Attraction*)(listItem_attraction->value))->scheduleList);
        params->paramsToFunctions = listItem_attraction->value;

        create_DetachThread(scheduleOpenCloseActiveWait, params, "Attraction scheduleOpenClose");
        //TODO: others ?
    }
}
