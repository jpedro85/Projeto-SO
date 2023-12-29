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
void beginRide(Attraction *attraction)
{

    struct timespec startRideTime;
    getActualTimespec(&startRideTime);
    // wait start conditions less then max time and min load
    int currentAttendance = 0;
    do
    {
        // TODO:Maybe a problem here Delay when trying to get lock
        lockMutex(&(attraction->currentAttendance_mut_t), "currentAttendance_mut_t");
        currentAttendance = attraction->currentAttendance;
        unlockMutex(&(attraction->currentAttendance_mut_t), "currentAttendance_mut_t");

        // TODO:currentAttendance > 0 ADD (not added because of tests)
    } while (attraction->isOpen && currentAttendance < attraction->attractionRideMinLoad && getCurrentTime_ms(startRideTime) <= attraction->rideBeginMaxWaitTime_ms);

    // if(attraction->isOpen){
    // }
    printSuccess("ride Began");
    EventInfo_AttractionRideEvent eventInfo;
    eventInfo.attractionName = attraction->name;

    writelock(&(attraction->isRunning_rwlock_t), "isRunning_rwlock_t");
    attraction->isRunning = true;
    rwlock_unlock(&(attraction->isRunning_rwlock_t), "isRunning_rwlock_t");

    writelock(&(attraction->rideCounter_rwlock_t), "rideCounter_rwlock_t");
    attraction->rideCounter++;
    eventInfo.rideNumber = attraction->rideCounter;
    rwlock_unlock(&(attraction->rideCounter_rwlock_t), "rideCounter_rwlock_t");

    eventInfo.usersInRide = currentAttendance;
    // TODO:Maybe a mutex is needed to check list.length
    eventInfo.usersWaiting = attraction->waitingLine.length;

    asyncCreateEvent_AttractionRideEvent(getCurrentSimulationDate(startTime, simulationConf.dayLength_s), eventInfo, RIDE_STARTED, 5, addMsgToQueue);
}

void endRide(Attraction *attraction)
{

    printSuccess("ride Ended");

    EventInfo_AttractionRideEvent eventInfo;
    eventInfo.attractionName = attraction->name;

    // TODO:Maybe a problem here Delay when trying to get lock
    lockMutex(&(attraction->currentAttendance_mut_t), "currentAttendance_mut_t");
    eventInfo.usersInRide = attraction->currentAttendance;
    unlockMutex(&(attraction->currentAttendance_mut_t), "currentAttendance_mut_t");

    // TODO:Maybe a mutex is needed to check list.length
    eventInfo.usersWaiting = attraction->waitingLine.length;

    writelock(&(attraction->isRunning_rwlock_t), "isRunning_rwlock_t");
    attraction->isRunning = false;
    rwlock_unlock(&(attraction->isRunning_rwlock_t), "isRunning_rwlock_t");

    readlock(&(attraction->rideCounter_rwlock_t), "rideCounter_rwlock_t");
    eventInfo.rideNumber = attraction->rideCounter;
    rwlock_unlock(&(attraction->rideCounter_rwlock_t), "rideCounter_rwlock_t");

    asyncCreateEvent_AttractionRideEvent(getCurrentSimulationDate(startTime, simulationConf.dayLength_s), eventInfo, RIDE_ENDED, 5, addMsgToQueue);
}

void *startRideCycle(void *param)
{

    printOption("startRideCycle Created");

    Attraction *attraction = (Attraction *)param;
    if (attraction->duration_ms != 0)
    {

        while (attraction->isOpen)
        {

            pthread_testcancel();
            beginRide(attraction);
            usleep(attraction->duration_ms * 1000);
            pthread_testcancel();
            printOption("AfterSleep");
            endRide(attraction);
        }

        if (attraction->isRunning)
        {
            writelock(&(attraction->isRunning_rwlock_t), "isRunning_rwlock_t");
            attraction->isRunning = false;
            rwlock_unlock(&(attraction->isRunning_rwlock_t), "isRunning_rwlock_t");
        }

        pthread_testcancel();
    }
    else
    {

        printSuccess("ride started");

        EventInfo_AttractionRideEvent eventInfo;
        eventInfo.attractionName = attraction->name;

        writelock(&(attraction->isRunning_rwlock_t), "isRunning_rwlock_t");
        attraction->isRunning = true;
        rwlock_unlock(&(attraction->isRunning_rwlock_t), "isRunning_rwlock_t");

        writelock(&(attraction->rideCounter_rwlock_t), "rideCounter_rwlock_t");
        attraction->rideCounter++;
        eventInfo.rideNumber = attraction->rideCounter;
        rwlock_unlock(&(attraction->rideCounter_rwlock_t), "rideCounter_rwlock_t");

        // TODO:Maybe a problem here Delay when trying to get lock
        lockMutex(&(attraction->currentAttendance_mut_t), "currentAttendance_mut_t");
        eventInfo.usersInRide = attraction->currentAttendance;
        unlockMutex(&(attraction->currentAttendance_mut_t), "currentAttendance_mut_t");

        // TODO:Maybe a mutex is needed to check list.length
        eventInfo.usersWaiting = attraction->waitingLine.length;
        asyncCreateEvent_AttractionRideEvent(getCurrentSimulationDate(startTime, simulationConf.dayLength_s), eventInfo, RIDE_STARTED, 5, addMsgToQueue);
    }

    printOption("startRideCycle exited");
    pthread_exit(NULL);
}

void stopRideCycle(Attraction *attraction)
{

    printOption("stopRideCycle called");

    // TODO: in user if duration_ms == 0 and isOpen is false user must leave the attraction
    if (attraction->duration_ms == 0)
    {

        printSuccess("ride ended");

        EventInfo_AttractionRideEvent eventInfo;
        eventInfo.attractionName = attraction->name;

        writelock(&(attraction->isRunning_rwlock_t), "isRunning_rwlock_t");
        attraction->isRunning = false;
        rwlock_unlock(&(attraction->isRunning_rwlock_t), "isRunning_rwlock_t");

        readlock(&(attraction->rideCounter_rwlock_t), "rideCounter_rwlock_t");
        eventInfo.rideNumber = attraction->rideCounter;
        rwlock_unlock(&(attraction->rideCounter_rwlock_t), "rideCounter_rwlock_t");

        // TODO:Maybe a problem here Delay when trying to get lock
        lockMutex(&(attraction->currentAttendance_mut_t), "currentAttendance_mut_t");
        eventInfo.usersInRide = attraction->currentAttendance;
        unlockMutex(&(attraction->currentAttendance_mut_t), "currentAttendance_mut_t");

        // TODO:Maybe a mutex is needed to check list.length
        eventInfo.usersWaiting = attraction->waitingLine.length;

        asyncCreateEvent_AttractionRideEvent(getCurrentSimulationDate(startTime, simulationConf.dayLength_s), eventInfo, RIDE_ENDED, 5, addMsgToQueue);
    }
    else
    {

        int error = pthread_cancel(attraction->rideCycles_thread);
        if (error != 0 && error != ESRCH)
            printFatalError("Can not cancel thread rideCycles_thread");

        if (attraction->isRunning)
        {

            EventInfo_AttractionRideEvent eventInfo;
            eventInfo.attractionName = attraction->name;

            writelock(&(attraction->isRunning_rwlock_t), "isRunning_rwlock_t");
            attraction->isRunning = false;
            rwlock_unlock(&(attraction->isRunning_rwlock_t), "isRunning_rwlock_t");

            readlock(&(attraction->rideCounter_rwlock_t), "rideCounter_rwlock_t");
            eventInfo.rideNumber = attraction->rideCounter;
            rwlock_unlock(&(attraction->rideCounter_rwlock_t), "rideCounter_rwlock_t");

            // TODO:Maybe a problem here Delay when trying to get lock
            lockMutex(&(attraction->currentAttendance_mut_t), "currentAttendance_mut_t");
            eventInfo.usersInRide = attraction->currentAttendance;
            unlockMutex(&(attraction->currentAttendance_mut_t), "currentAttendance_mut_t");

            // TODO:Maybe a mutex is needed to check list.length
            eventInfo.usersWaiting = attraction->waitingLine.length;

            asyncCreateEvent_AttractionRideEvent(getCurrentSimulationDate(startTime, simulationConf.dayLength_s), eventInfo, RIDE_ENDED, 5, addMsgToQueue);

            printSuccess("ride ended2");
        }
    }
}

void openAttraction(void *attractionP)
{

    printWarning("openAttraction called");

    Attraction *attraction = (Attraction *)attractionP;

    writelock(&(attraction->isOpen_rwlock_t), "isOpen_rwlock_t");
    attraction->isOpen = true;
    rwlock_unlock(&(attraction->isOpen_rwlock_t), "isOpen_rwlock_t");

    writelock(&(attraction->rideCounter_rwlock_t), "rideCounter_rwlock_t");
    attraction->rideCounter = 0;
    rwlock_unlock(&(attraction->rideCounter_rwlock_t), "rideCounter_rwlock_t");

    EventInfo_AttractionEvent eventInfo;
    eventInfo.attractionName = attraction->name;
    asyncCreateEvent_AttractionEvent(getCurrentSimulationDate(startTime, simulationConf.dayLength_s), eventInfo, ATTRACTION_OPEN, 4, addMsgToQueue);
    //    // create_DetachThread(startRideCycle,attractionP,"startRideCycle");
    if (pthread_create(&(attraction->rideCycles_thread), NULL, startRideCycle, attractionP) < 0)
    {
        printError(attraction->name);
        printFatalError("Could not creat thread for startRideCycle.");
    }
}

void closeAttraction(void *attractionP)
{

    printWarning("CloseAttraction called");

    Attraction *attraction = (Attraction *)attractionP;
    writelock(&(attraction->isOpen_rwlock_t), "isOpen_rwlock_t");
    attraction->isOpen = false;
    rwlock_unlock(&(attraction->isOpen_rwlock_t), "isOpen_rwlock_t");

    stopRideCycle(attraction);

    EventInfo_AttractionEvent eventInfo;
    eventInfo.attractionName = attraction->name;
    asyncCreateEvent_AttractionEvent(getCurrentSimulationDate(startTime, simulationConf.dayLength_s), eventInfo, ATTRACTION_CLOSED, 4, addMsgToQueue);
}

void startAttraction(void *attractionP)
{

    Attraction *attraction = (Attraction *)attractionP;
    attraction->isOpen = false;
    // init the lock;
    rwlock_init(&(attraction->isOpen_rwlock_t), "isOpen_rwlock_t");
    rwlock_init(&(attraction->isRunning_rwlock_t), "isRunning_rwlock_t");
    rwlock_init(&(attraction->rideCounter_rwlock_t), "rideCounter_rwlock_t");

    attraction->isRunning = false;
    attraction->currentAttendance = 0;
    attraction->rideCounter = 0;
    mutex_init(&(attraction->currentAttendance_mut_t), "currentAttendance_mut_t");
    mutex_init(&(attraction->waitingLine_mutex_t), "waitingLine_mutex_t");
    semInit(&(attraction->enterRide_sem_t), attraction->rideCapacity, "enterRide_sem_t");
}

/**
 * The function starts the simulation for each attraction in the park by initializing the attraction,
 * creating a thread to handle the opening and closing schedule.
 */
void startAttractionSimulation()
{

    ListItem *listItem_attraction;
    ForEach_LinkedList((&(park.attractions)), listItem_attraction)
    {
        // init attraction
        startAttraction(listItem_attraction->value);

        ScheduleParams *params = (ScheduleParams *)malloc(sizeof(ScheduleParams));
        params->openFunction = openAttraction;
        params->closeFunction = closeAttraction;
        params->scheduleList = &(((Attraction *)(listItem_attraction->value))->scheduleList);
        params->paramsToFunctions = listItem_attraction->value;

        create_DetachThread(scheduleOpenCloseActiveWait, params, "Attraction scheduleOpenClose");
        // TODO: others ?
    }
}

void enterAttraction(User *client, Attraction *attraction)
{
    EventInfo_UserEventWaitingLine eventInfo;

    lockMutex(&attraction->waitingLine_mutex_t, "waitingLine_mutex_t");
    addValue_LinkedList(&(attraction->waitingLine), client);
    eventInfo.lineSize = attraction->waitingLine.length;
    unlockMutex(&attraction->waitingLine_mutex_t, "waitingLine_mutex_t");

    eventInfo.clientID = client->id;
    eventInfo.attractionName = attraction->name;

    client->state = IN_WAITING_LINE;
    client->currentAttraction = attraction;
    asyncCreateEvent_UserEventWaitingLine(getCurrentSimulationDate(startTime, simulationConf.dayLength_s), eventInfo, ENTERING_WAITING_LINE, 5, addMsgToQueue);
}

void enterAttractionRide(User *client, Attraction *attraction)
{
    printSuccess("User entered ride");

    lockMutex(&(attraction->waitingLine_mutex_t), "waitingLine_mutex_t");
    // TODO: Verify requirements if all users that enter the ride are in the first position of the list. Before calling wait at the semaphore.
    removeItemByIndex_LinkedList(&(attraction->waitingLine), 0);
    unlockMutex(&(attraction->waitingLine_mutex_t), "waitingLine_mutex_t");

    int currentAttendance = 0;
    lockMutex(&(attraction->currentAttendance_mut_t), "currentAttendance_mut_t");
    attraction->currentAttendance++;
    currentAttendance=attraction->currentAttendance;
    unlockMutex(&(attraction->currentAttendance_mut_t), "currentAttendance_mut_t");

    if(currentAttendance<attraction->rideCapacity){
        semPost(&(attraction->enterRide_sem_t), "enterRide_sem_t");
    }

    EventInfo_UserRideEvent eventInfo;
    eventInfo.clientID = client->id;
    eventInfo.attractionName = attraction->name;

    client->state = IN_RIDE;

    readlock(&(attraction->rideCounter_rwlock_t), "rideCounter_rwlock_t");
    eventInfo.rideNumber = attraction->rideCounter;
    rwlock_unlock(&(attraction->rideCounter_rwlock_t), "rideCounter_rwlock_t");

    asyncCreateEvent_UserRideEvent(getCurrentSimulationDate(startTime, simulationConf.dayLength_s), eventInfo, ENTERING_RIDE, 5, addMsgToQueue);
}

void leaveAttraction_waitingLine(User *client, Attraction *attraction)
{

    EventInfo_UserEventWaitingLine eventInfo;
    ListItem *attractionItem;
    lockMutex(&attraction->waitingLine_mutex_t, "waitingLine_mutex_t");

    int index = 0;
    ForEach_LinkedList((&(attraction->waitingLine)), attractionItem)
    {

        if (((User *)(attractionItem->value))->id == client->id)
        {
            removeItemByIndex_LinkedList(&(attraction->waitingLine), index);
            break;
        }
        index++;
    }

    eventInfo.lineSize = attraction->waitingLine.length;
    unlockMutex(&attraction->waitingLine_mutex_t, "waitingLine_mutex_t");

    eventInfo.clientID = client->id;
    eventInfo.attractionName = attraction->name;

    client->state = IN_PARK;
    client->currentAttraction = NULL;

    asyncCreateEvent_UserEventWaitingLine(getCurrentSimulationDate(startTime, simulationConf.dayLength_s), eventInfo, LEAVING_WAITING_LINE, 5, addMsgToQueue);
}

void leaveAttraction_ride(User *client, Attraction *attraction)
{
    printSuccess("User leaving ride");

    EventInfo_UserRideEvent eventInfo;
    eventInfo.clientID = client->id;
    eventInfo.attractionName = attraction->name;

    client->state = IN_PARK;
    client->currentAttraction = NULL;


    int currentAttendance=0;
    lockMutex(&(attraction->currentAttendance_mut_t), "currentAttendance_mut_t");
    attraction->currentAttendance--;
    currentAttendance=attraction->currentAttendance;
    unlockMutex(&(attraction->currentAttendance_mut_t), "currentAttendance_mut_t");

    if(currentAttendance<attraction->rideCapacity){
        semPost(&(attraction->enterRide_sem_t), "enterRide_sem_t");
    }

    readlock(&(attraction->rideCounter_rwlock_t), "rideCounter_rwlock_t");
    eventInfo.rideNumber = attraction->rideCounter;
    rwlock_unlock(&(attraction->rideCounter_rwlock_t), "rideCounter_rwlock_t");

    asyncCreateEvent_UserRideEvent(getCurrentSimulationDate(startTime, simulationConf.dayLength_s), eventInfo, LEAVING_RIDE, 5, addMsgToQueue);
}

/**
 * The function leaveAttraction is used to handle the process of a user leaving an attraction.
 *
 * @param client A pointer to the User object representing the client who wants to leave the
 * attraction.
 * @param attraction The "attraction" parameter is a pointer to an object of type "Attraction". It
 * represents the attraction from which the user wants to leave.
 */
void leaveAttraction(User *client, Attraction *attraction)
{

    if (client->state == IN_WAITING_LINE)
    {
        leaveAttraction_waitingLine(client, attraction);
    }
    else if (client->state == IN_RIDE)
    {
        leaveAttraction_ride(client, attraction);
    }
    else
    {
        EvenInfo_SimulationError eventInfo;
        eventInfo.errorMsg = "Incorrect State in leaveAttraction";
        eventInfo.errorValue = 0;
        asyncCreateEvent_SimulationError(getCurrentSimulationDate(startTime, simulationConf.dayLength_s), eventInfo, 5, addMsgToQueue);
    }
}

int isAttractionRunning(Attraction *attraction)
{

    int error = try_rdlock(&(attraction->isRunning_rwlock_t), "isRunning_rwlock_t");

    if (error == EBUSY)
        return error;
    else
    {
        int running = attraction->isRunning;
        rwlock_unlock(&(attraction->isRunning_rwlock_t), "isRunning_rwlock_t");
        return running;
    }
}