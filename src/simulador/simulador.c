#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "file_loadConf.h"
#include "../common/linked_list.h"
#include "../common/consoleAddons.h"
#include "../common/cjson/cJSON.h"
#include "socketServer.h"

Park park;
SimulationConf simulationConf;

int main( int argc , char *argv[] ){

    int argUsed = 1;
    
    if ( fork() == 0){

        char* argvToPass[] = {"./build/monitor",NULL};
        int a = execvp("./build/monitor",argvToPass);

        printf("\033[1;36masdasd:%d\033[1;0m\n",a);
        
    } else {

        char* simulationConfFile = "simuladorDefault.conf";;
        if(argc > 1)
            simulationConfFile = argv[1];


        loadConfig(&park,&simulationConf,simulationConfFile);

        startServer();


        
        printSuccess("Simulador Terminated Successfully");

    }

    
    return 0;
}