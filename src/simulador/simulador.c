#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "file_loadConf.h"
#include "../common/linked_list.h"
#include "../common/consoleAddons.h"
#include "../common/cjson/cJSON.h"
#include "socketServer/socketServer.h"

Park park;
SimulationConf simulationConf;

int main( int argc , char *argv[] ){
    
    // reading args
    char* simulationConfFile = "simuladorDefault.conf";;
    if(argc > 1)
        simulationConfFile = argv[1];

    loadConfig(&park,&simulationConf,simulationConfFile);

    startServer();

    //  starting monitor
    // int error = fork();
    // if ( error == 0){
    //     char* argvToPass[] = {"./build/monitor",NULL};
    //     int a = execvp("./build/monitor",argvToPass);

    // } else if( error < 0) 
    //     printError("Couldn't start monitor.");

    waitFirstConnection();

    sleep(10);
    printInfo("endSleep");

    addMsgToQueue("From Server Client is connected.1234567890assssssssssssss");
    sleep(1);
    addMsgToQueue("From Server second message.fffff");
    sleep(1);
    addMsgToQueue("From Server third message.");
    sleep(1);
    addMsgToQueue("From Server 4 message.");
    sleep(1);
    addMsgToQueue("From Server 5 message. message message message message");
    
    printInfo("Sleeping");
    sleep(5);
    printInfo("endSleep");

    printInfo("simulating.");
    stopServer();
    printSuccess("Simulador Terminated Successfully");

    return 0;
}