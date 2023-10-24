#include <stdlib.h>
#include <stdio.h>

#include "park.h"
#include "simulationConf.h"
#include "attraction.h"
#include "../file.h"
#include "../consoleAddons.h"


int loadSimulationConfig(cJSON* cJsonObject ,SimulationConf* simulationConf){

   cJSON* simulation =  cJSON_GetObjectItem(cJsonObject,"simulation");
   int errorCount = 0;

    if(simulation){

        char* aa;

        errorCount += loadItemNumber(simulation,"averageClientArriveTime_ms",&(simulationConf->averageClientArriveTime_ms));
        errorCount += loadItemNumber(simulation,"toleranceClientArriveTime_ms",&(simulationConf->toleranceClientArriveTime_ms));
        errorCount += loadItemNumber(simulation,"dayLength_s",&(simulationConf->dayLength_s));
        errorCount += loadItemNumber(simulation,"userMaxAge",&(simulationConf->userMaxAge));
        errorCount += loadItemNumber(simulation,"userMinAge",&(simulationConf->userMinAge));
        errorCount += loadItemNumber(simulation,"userMinWaitingTime_ms",&(simulationConf->userMinWaitingTime_ms));

        
        if(errorCount > 0){
            char str[55];
            sprintf(str, "Simulation configuration Loaded with %d errors!", errorCount);
            printWarning(str);
        }

    }
    
}

void loadPark(Park* park,cJSON* fileOpen_Object){

}

Attraction* loadAttraction(cJSON* attraction_Object ){

}


void loadConfig(Park* park,SimulationConf* simulationConf, char* configFile){

    
    cJSON* fileOpen_Object = loadAndParseJSONFromFile("simuladorDefault.conf");

    if(!fileOpen_Object){

        printError("The configuration could not be loaded.");
        printWarning("Using default simulator configuration.");

        fileOpen_Object = loadAndParseJSONFromFile("simuladorDefault.conf");

        if(!fileOpen_Object){
            printError("The default configuration could not be loaded. The program will exit.");
            exit(1);
        }

    }

    loadSimulationConfig(fileOpen_Object,simulationConf);

    printf("%d" , simulationConf->averageClientArriveTime_ms);



}










