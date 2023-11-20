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

void startSimulation() {        
    
    startServer();  
    waitFirstConnection();
    clock_gettime(CLOCK_REALTIME, &startTime);

    int threadError = pthread_create(&simulationStartThread, NULL, createParkClients, NULL);
    if (threadError == -1) {
        printFatalError("Could not start the simulation");
    }

    // Initialize and start the attraction simulation thread
    pthread_t attractionThread;
    pthread_create(&attractionThread, NULL, simulateAttraction, &attraction);
    
    pthread_join(simulationStartThread, 0);
    pthread_join(attractionThread, NULL);
}


void stopSimulation()
{
}