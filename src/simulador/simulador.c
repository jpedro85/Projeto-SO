#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include "file_loadConf.h"
#include "../common/linked_list.h"
#include "../common/consoleAddons.h"
#include "../common/cjson/cJSON.h"
#include "socketServer.h"
#include "user.h"

extern Park park;
extern SimulationConf simulationConf;

pthread_t simulationStartThread;

void startSimulation();
void endSimulation();

int main(int argc, char *argv[])
{
    // Initialize the random generator
    srand((unsigned)time(NULL));

    // // reading args
    char *simulationConfFile = "simuladorDefault.conf";
    ;
    if (argc > 1)
        simulationConfFile = argv[1];

    loadConfig(&park, &simulationConf, simulationConfFile);

    startSimulation();

    // startServer();

    // //  starting monitor
    // // int error = fork();
    // // if ( error == 0){
    // //     char* argvToPass[] = {"./build/monitor",NULL};
    // //     int a = execvp("./build/monitor",argvToPass);

    // // } else if( error < 0)
    // //     printError("Couldn't start monitor.");

    // waitFirstConnection();

    // sleep(10);
    // printInfo("endSleep");

    // addMsgToQueue("From Server Client is connected.1234567890");
    // sleep(2);
    // addMsgToQueue("From Server second message.");
    // sleep(2);
    // addMsgToQueue("From Server third message.");

    // printInfo("Sleeping");
    // sleep(10);
    // printInfo("endSleep");

    // printInfo("simulating.");
    // stopServer();
    // printSuccess("Simulador Terminated Successfully");

    return 0;
}

void startSimulation()
{
    int threadError = pthread_create(&simulationStartThread, NULL, createParkClients, NULL);
    if (threadError == -1)
    {
        printf("Could not start the simulation");
    }
    else
    {
        printf("Simulation Started");
        pthread_join(simulationStartThread,0);
    }
}

void stopSimulation()
{
}