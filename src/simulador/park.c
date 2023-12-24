#include <stdlib.h>
#include <stdio.h>

#include "globals.h"
#include "schedule.h"
#include "../common/mutexAddons.h"
#include "../common/consoleAddons.h"

int dayCounter = 0;
/**
 * The function "openPark" sets the "isOpen" variable in the "park" struct to true, while using a
 * read-write lock to ensure thread safety.
 * 
 * @param param The parameter "param" is a void pointer, which means it can point to any type of data.
 * In this case, it is not being used in the function and its purpose is unclear. It could potentially
 * be used to pass additional information or data to the function, but without more context it is
 */
void openPark(void* param){

    writelock(&(park.parkIsOpen_rwlock_t),"parkIsOpen_rwlock_t");
    park.isOpen = true;
    rwlock_unlock(&(park.parkIsOpen_rwlock_t),"parkIsOpen_rwlock_t");

    printOption("openPark called");
    //TODO: add correct Event PARK_OPEN
}

/**
 * The closePark function sets the isOpen variable in the park struct to false, indicating that the
 * park is closed.
 * 
 * @param param The parameter `param` is a void pointer, which means it can point to any type of data.
 * In this case, it is not used in the function and can be ignored.
 */
void closePark(void* param){
    
    writelock(&(park.parkIsOpen_rwlock_t),"parkIsOpen_rwlock_t");
    park.isOpen = false;
    rwlock_unlock(&(park.parkIsOpen_rwlock_t),"parkIsOpen_rwlock_t");

    printOption("closePark called");

    dayCounter++;

    if(dayCounter == simulationConf.numberOfDaysToSimulate){
        writelock(&simulationStatus_rwlock_t,"simulationStatus_rwlock_t");
        simulationStatus = ENDED;
        rwlock_unlock(&simulationStatus_rwlock_t,"simulationStatus_rwlock_t");
        //TODO: add correct Event SIMULATION ENDED
    }
    //TODO: add correct Event PARK_CLOSED
}

/**
 * The function initializes the park by setting its capacity and status.
 */
void startPark(){
    semInit(&(park.parkVacancy_sem_t),park.parkCapacity,"parkVacancy_sem_t");
    park.isOpen = false;
    dayCounter = 0;
}

/**
 * The function "startParkSimulation" starts a park simulation by calling the "startPark" function and
 * creating a detached thread for the "scheduleOpenCloseActiveWait" function.
 */
void startParkSimulation(){

    startPark();

    ScheduleParams* params = (ScheduleParams*)malloc(sizeof(ScheduleParams));
    params->openFunction = openPark;
    params->closeFunction = closePark;
    params->scheduleList = &(park.scheduleList);
    params->paramsToFunctions = NULL;

    create_DetachThread(scheduleOpenCloseActiveWait, params, "Attraction scheduleOpenClose");    
}
