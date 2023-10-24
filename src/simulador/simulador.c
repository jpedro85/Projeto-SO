#include <stdio.h>
#include <stdlib.h>
//#include <unistd.h>

#include "file_loadConf.h"
#include "../linked_list.h"
#include "../consoleAddons.h"
#include "../cjson/cJSON.h"

int main( int argc , char *argv[] ){

    // int argUsed = 1;
    
    // if ( fork() == 0){

    //     int argToPassCount = argc-argUsed + 2;

    //     char** argvToPass = (char**)malloc( argToPassCount * sizeof(char*));

    //     argvToPass[0] = "../monitor/monitor";

    //     int j = argUsed;
    //     for(int i = 0; i < argToPassCount-1 && j<argc ; i++){
    //         argvToPass[i] = argv[j];
    //         j++;
    //     }

    //     argvToPass[ argToPassCount - 1 ] = NULL;

    //     exec(argvToPass[0],argvToPass);
    // }

    
    Park park;
    SimulationConf simulationConf;
    char* simulationConfFile = "simuladorDefault.conf";

    loadConfig(&park,&simulationConf,simulationConfFile);



    return 0;
}