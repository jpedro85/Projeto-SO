#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include "file_loadConf.h"
#include "../common/linked_list.h"
#include "../common/consoleAddons.h"
#include "../common/mutexAddons.h"
#include "../common/cjson/cJSON.h"
#include "socketServer/socketServer.h"
#include "globals.h"
#include "user.h"
#include "park.h"
#include "../common/events.h"
#include "schedule.h"

pthread_t simulationStartThread;

void startSimulation();
void stopSimulation();
void endSimulation();
void startCountingDays();
void startSimulator();

int main(int argc, char *argv[])
{
    // Initialize the random generator
    srand((unsigned)time(NULL));

    char *simulationConfFile = "simuladorDefault.conf";

    if (argc > 1)
        simulationConfFile = argv[1];

    system("clear");
    loadConfig(&park, &simulationConf, simulationConfFile);
    int erroCount = verifyLoadedValues(&park, &simulationConf);

    if(erroCount > 0){
        printf("\033[1;31mLoaded configuration contains with %d errors!\033[1;0m\n",erroCount ); 
        printFatalError("");
    }

    startSimulator();

    startSimulation();

    stopServer();
    printSuccess("Simulador Terminated Successfully");

    return 0;
}

void startSimulator(){

    startServer();
    waitFirstConnection();
}

void startSimulation()
{   
    simulationStatus = STARTING;
    rwlock_init(&simulationStatus_rwlock_t,"simulationStatus_rwlock_t");
    
    startParkSimulation();
    startAttractionSimulation();

    writelock(&simulationStatus_rwlock_t,"simulationStatus_rwlock_t");
    clock_gettime(CLOCK_REALTIME,&startTime);
    simulationStatus = RUNNING;
    printInfo("Started");
    rwlock_unlock(&simulationStatus_rwlock_t,"simulationStatus_rwlock_t");

    Event event = createEvent(SIMULATOR_EVENT,SIMULATION_STARTED,getCurrentSimulationDate(startTime,simulationConf.dayLength_s));
    addMsgToQueue(eventToJSON_String(event,0));
    
    activeWaitUntilEndOfDay(simulationConf.numberOfDaysToSimulate);
    Date date = getCurrentSimulationDate(startTime,simulationConf.dayLength_s);

    writelock(&simulationStatus_rwlock_t,"simulationStatus_rwlock_t");
    simulationStatus = ENDED;
    printInfo("Ended");
    rwlock_unlock(&simulationStatus_rwlock_t,"simulationStatus_rwlock_t");

    Event event2 = createEvent(SIMULATOR_EVENT,SIMULATION_ENDED,date);
    addMsgToQueue(eventToJSON_String(event2,0));
    
    //wait for last sends ?
    sleep(20);
    //TODO:wait last mesg on sever insted of sleep

     //initParkSemaphores();

    // int threadError = pthread_create(&simulationStartThread, NULL, createParkClients, NULL);
    // if (threadError == -1)
    // {
    //     printFatalError("Could not start the simulation");
    // }
    
    // pthread_join(simulationStartThread, 0);
    
}


// void startCountingDays(){

//     //starting counting time;
//     clock_gettime(CLOCK_REALTIME,&startTime);
//     dayCounter = 0;

//     rwlock_init(&dayStartedCondition_rwlock_t,"dayStartedCondition_rwlock_t");

//     printInfo("in:");
//     //update day until simulationConf.numberOfDaysToSimulate day 0 is the first
//     for (int day = 1; day <= simulationConf.numberOfDaysToSimulate ; day++)
//     {
//         writelock(&dayStartedCondition_rwlock_t,"dayStartedCondition_rwlock_t");
//         dayCounter = day;
//         rwlock_unlock(&dayStartedCondition_rwlock_t,"dayStartedCondition_rwlock_t");
//         printf("%d \n",dayCounter);

//        // clock_gettime(CLOCK_REALTIME,&currentDayStartTime);
//         //signal all threads waiting on dayStarted_cond_t
//         // if(pthread_cond_broadcast(&dayStarted_cond_t) !=0)
//         //     printFatalError("Can not broadCast to dayStarted_cond_t");
       
//         sleep(simulationConf.dayLength_s);
//         //TODO: verify needing mutex for changing dayCounter and currentDayStartTime
//         //TODO: verify last days conditions

        
//     }
     
//     printInfo("Ended");

//     sleep(10);
//     stopSimulation();

// }

void stopSimulation()
{
}

