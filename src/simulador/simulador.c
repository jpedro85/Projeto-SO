#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <execinfo.h>

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

void segmentationFaultHandler(int signalNumber){
    void *array[10];
    size_t size;
    FILE *f;

    // Open a file to write the addresses and symbols
    f = fopen("stack_trace.txt", "w");
    if (f == NULL) {
        printError("Failed to open stack_trace.txt");
        exit(EXIT_FAILURE);
    }

    // Get void*'s for all entries on the stack
    size = backtrace(array, 10);

    // Write out all the frames to the file
    char **symbols = backtrace_symbols(array, size);
    for (size_t i = 0; i < size; i++) {
        fprintf(f, "%s\n", symbols[i]);
    }

    free(symbols);
    fclose(f);
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
    signal(SIGSEGV,segmentationFaultHandler);
    
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
    

    writelock(&simulationStatus_rwlock_t,"simulationStatus_rwlock_t");
    clock_gettime(CLOCK_REALTIME,&startTime);
    startParkSimulation();
    startAttractionSimulation();
    startClientSimulation();
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
    sleep(5);
}
void stopSimulation()
{
}