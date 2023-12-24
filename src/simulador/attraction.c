#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "globals.h"
#include "../common/consoleAddons.h"
#include "../common/linked_list.h"
#include "../common/date.h"
#include "../common/mutexAddons.h"
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
        lockMutex(&(attraction->currentAttendance_mut_t),"currentAttendance_mut_t");
        currentAttendance = attraction->currentAttendance;
        unlockMutex(&(attraction->currentAttendance_mut_t),"currentAttendance_mut_t");

    }while( attraction->isOpen && currentAttendance < attraction->attractionRideMinLoad && getCurrentTime_ms(startRideTime) <= attraction->rideBeginMaxWaitTime_ms);

    if(attraction->isOpen){
        writelock(&(attraction->isRunning_rwlock_t),"isRunning_rwlock_t");
        attraction->isRunning = true;
        rwlock_unlock(&(attraction->isRunning_rwlock_t),"isRunning_rwlock_t");
    }

    printSuccess("ride Began");
}

void endRide(Attraction* attraction ){

    writelock(&(attraction->isRunning_rwlock_t),"isRunning_rwlock_t");
    attraction->isRunning = false;
    rwlock_unlock(&(attraction->isRunning_rwlock_t),"isRunning_rwlock_t");

    printSuccess("ride Ended");
}

void* startRideCycle(void* param ){

    printOption("startRideCycle Created");

    Attraction* attraction = (Attraction*)param;
    if(attraction->duration_ms != 0){

        while(attraction->isOpen){

            pthread_testcancel();
            beginRide(attraction);
            usleep(attraction->duration_ms * 1000);
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
                
        writelock(&(attraction->isRunning_rwlock_t),"isRunning_rwlock_t");
        attraction->isRunning = true;
        rwlock_unlock(&(attraction->isRunning_rwlock_t),"isRunning_rwlock_t");
    }

    printOption("startRideCycle exited");
    pthread_exit(NULL);
}

void stopRideCycle(Attraction* attraction ){

    printOption("stopRideCycle called");

    //TODO: in user if duration_ms == 0 and isOpen is false user must leave the attraction
    if(attraction->duration_ms == 0){
        writelock(&(attraction->isRunning_rwlock_t),"isRunning_rwlock_t");
        attraction->isRunning = false;
        rwlock_unlock(&(attraction->isRunning_rwlock_t),"isRunning_rwlock_t");

        printSuccess("ride ended");
    }else{

        int error = pthread_cancel(attraction->rideCycles_thread);
        if(error != 0 && error != ESRCH)
            printFatalError("Can not cancel thread rideCycles_thread");

        if(attraction->isRunning){
            writelock(&(attraction->isRunning_rwlock_t),"isRunning_rwlock_t");
            attraction->isRunning = false;
            rwlock_unlock(&(attraction->isRunning_rwlock_t),"isRunning_rwlock_t");
        }

        printSuccess("ride ended2");
    }
}


void openAttraction(void* attractionP){

    printWarning("openAttraction called");

    Attraction* attraction = (Attraction*)attractionP;
    writelock(&(attraction->isOpen_rwlock_t),"isOpen_rwlock_t");
    attraction->isOpen = true;
    rwlock_unlock(&(attraction->isOpen_rwlock_t),"isOpen_rwlock_t");

   // create_DetachThread(startRideCycle,attractionP,"startRideCycle");
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
