#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>

#include "file_loadConf.h"
#include "../common/linked_list.h"
#include "../common/consoleAddons.h"
#include "../common/cjson/cJSON.h"
#include "socketServer/socketServer.h"
#include "globals.h"
#include "user.h"
#include "park.h"
#include "../common/events.h"

pthread_t simulationStartThread;

void startSimulation();
void endSimulation();

int main(int argc, char *argv[])
{
    // Initialize the random generator
    srand((unsigned)time(NULL));

    char *simulationConfFile = "simuladorDefault.conf";

    if (argc > 1)
        simulationConfFile = argv[1];

    system("clear");
    loadConfig(&park, &simulationConf, simulationConfFile);

    initParkSemaphores();
    startSimulation();

    stopServer();
    printSuccess("Simulador Terminated Successfully");

    return 0;
}

void startSimulation()
{   
    startServer();
    waitFirstConnection();
    clock_gettime(CLOCK_REALTIME,&startTime);

    EventInfo_UserEventPark eventInfo;
    eventInfo.clientID = 0;
    asyncCreateEvent_UserEventPark( getCurrentSimulationDate(startTime,simulationConf.dayLength_s), eventInfo, 0, sizeof(eventInfo), addMsgToQueue);
    
    sleep(2);
    
    // int threadError = pthread_create(&simulationStartThread, NULL, createParkClients, NULL);
    // if (threadError == -1)
    // {
    //     printFatalError("Could not start the simulation");
    // }
    
    pthread_join(simulationStartThread, 0);
    
}

void stopSimulation()
{
}