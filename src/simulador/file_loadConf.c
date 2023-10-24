#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "park.h"
#include "simulationConf.h"
#include "attraction.h"
#include "../file.h"
#include "../consoleAddons.h"


int loadItemNumber(cJSON* object,char* name,char* error){

    cJSON* json_item = cJSON_GetObjectItemCaseSensitive(object,name);
    if(!json_item)
        return cJSON_GetNumberValue(json_item);
    else
        printError(error);

    return 0;
}

int loadSimulationConfig(cJSON* cJsonObject ,SimulationConf* simulationConf){

   cJSON* simulation =  cJSON_GetObjectItemCaseSensitive(cJsonObject,"simulation");

//    simulation: {
//         averageClientArriveTime_ms : 10,
//         toleranceClientArriveTime_ms : 1,
//         dayLength_s: 12
//         userMaxAge: 80,
//         userMinAge: 0,
//         userMinWaitingTime_ms: 10
//     },


   if(simulation != NULL){

        simulationConf->averageClientArriveTime_ms = loadItemNumber(simulation,"averageClientArriveTime_ms","Could not load averageClientArriveTime_ms.");

        
   }
    
}

void loadPark(Park* park,cJSON* fileOpen_Object){

}

Attraction* loadAttraction(cJSON* attraction_Object ){

}


void loadConfig(Park* park,SimulationConf* simulationConf, char* configFile){

    
    cJSON* fileOpen_Object = loadAndParseJSONFromFile("simuladorDefault.conf");

    if(!fileOpen_Object){

        char* str;
        sprintf (str,"The configuration %s could not be loaded.","change");
        printError(str);
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










