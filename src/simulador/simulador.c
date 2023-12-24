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
    
    int status = -1;
    while(true)
    {
        readlock(&simulationStatus_rwlock_t,"simulationStatus_rwlock_t");
        status = simulationStatus;
        rwlock_unlock(&simulationStatus_rwlock_t,"simulationStatus_rwlock_t");

        if(status == ENDED)
            return;
    }
    

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

