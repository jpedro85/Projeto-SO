#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "park.h"
#include "simulationConf.h"
#include "attraction.h"
#include "schedule.h"
#include "../file.h"
#include "../consoleAddons.h"

int loadAttraction(cJSON* attraction_Object, Attraction* Attraction){

    if(attraction_Object){

        int errorCount = 0;

        errorCount += loadItemString(attraction_Object,"name",&(Attraction->name));
        errorCount += loadItemNumber(attraction_Object,"minAge",&(Attraction->minAge));
        errorCount += loadItemNumber(attraction_Object,"maxAge",&(Attraction->maxAge));
        errorCount += loadItemNumber(attraction_Object,"duration_ms",&(Attraction->duration_ms));
        errorCount += loadItemNumber(attraction_Object,"rideCapacity",&(Attraction->rideCapacity));
        errorCount += loadItemLinkedListSchedule(attraction_Object,"scheduleList",&(Attraction->scheduleList));

        return errorCount;

    } else
        return 1;
}

int loadItemLinkedListAttraction( cJSON* cJsonObject, char* objectName, LinkedList* linkedList ){

    printf("\033[1;37mLoading array %s.\033[1;0m\n",objectName); 
    cJSON* arrayAttraction=  cJSON_GetObjectItem(cJsonObject,objectName);

    if(arrayAttraction && cJSON_IsArray(arrayAttraction)){

        cJSON* item;
        Attraction* attraction;
        int errorCount = 0;

        cJSON_ArrayForEach(item,arrayAttraction){

            attraction = (Attraction*)malloc(sizeof(Attraction));

            errorCount += loadAttraction(item,attraction);

            if(errorCount > 0){
                printError("Could not load array");
                clear_linkedList(linkedList);
                return 1;
            }else{
                addValue_LinkedList(linkedList,attraction);
            }

        }

        printInfo("Array Loaded");
        return 0;

    } else
        printf("\033[1;31mCan not load %s.\033[1;0m\n",objectName); 

    return 1;
}

int loadItemLinkedListSchedule( cJSON* cJsonObject, char* objectName, LinkedList* linkedList ){

    printf("\033[1;37mLoading array %s.\033[1;0m\n",objectName); 
    cJSON* arraySchedule =  cJSON_GetObjectItem(cJsonObject,objectName);

    if(arraySchedule && cJSON_IsArray(arraySchedule)){

        cJSON* item;
        Schedule* schedule;
        int errorCount = 0;

        cJSON_ArrayForEach(item,arraySchedule){

            schedule = (Schedule*)malloc(sizeof(Schedule));

            errorCount += loadItemNumber(item,"begin",&(schedule->begin));
            errorCount += loadItemNumber(item,"end",&(schedule->begin));
            schedule->done = false;

            if(errorCount > 0){
                printError("Could not load array");
                clear_linkedList(linkedList);
                return 1;
            }else{
                addValue_LinkedList(linkedList,schedule);
            }

        }

        printInfo("Array Loaded");
        return 0;

    } else
        printf("\033[1;31mCan not load %s.\033[1;0m\n",objectName); 

    return 1;
}   


int loadSimulationConfig(cJSON* cJsonObject ,SimulationConf* simulationConf){

    printInfo("Loading simulation object.");

    cJSON* simulation =  cJSON_GetObjectItem(cJsonObject,"simulation");
    int errorCount = 0;

    if(simulation){

        errorCount += loadItemNumber(simulation,"averageClientArriveTime_ms",&(simulationConf->averageClientArriveTime_ms));
        errorCount += loadItemNumber(simulation,"toleranceClientArriveTime_ms",&(simulationConf->toleranceClientArriveTime_ms));
        errorCount += loadItemNumber(simulation,"dayLength_s",&(simulationConf->dayLength_s));
        errorCount += loadItemNumber(simulation,"userMaxAge",&(simulationConf->userMaxAge));
        errorCount += loadItemNumber(simulation,"userMinAge",&(simulationConf->userMinAge));
        errorCount += loadItemNumber(simulation,"userMinWaitingTime_ms",&(simulationConf->userMinWaitingTime_ms));

        if(errorCount > 0){
            printf("\033[1;33mSimulation object Loaded with %d errors!\033[1;0m\n",errorCount);
        }else{
            printInfo("Loaded simulation object successfully.");
            return 0;
        }

    }else
        printError("Cannot locate simulation object.");
    
    return 1;
}

int loadPark(cJSON* cJsonObject, Park* park){

    printInfo("Loading park object.");
    cJSON* cJson_park =  cJSON_GetObjectItem(cJsonObject,"park");
    int errorCount = 0;

    if(cJson_park){

        errorCount += loadItemString(cJson_park,"name",&(park->name));
        errorCount += loadItemNumber(cJson_park,"parkCapacity",&(park->parkCapacity));
        errorCount += loadItemLinkedListSchedule(cJson_park,"scheduleList",&(park->scheduleList));
        errorCount += loadItemLinkedListAttraction(cJson_park,"parkAttractions",&(park->attractions));

        if(errorCount > 0){
            printf("\033[1;33mPark object Loaded with %d errors!\033[1;0m\n",errorCount);
        }else{
            printInfo("Loaded Park object successfully.");
            return 0;
        }

    }else
        printError("Cannot locate park object.");
    
    return 1;
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

    int errorCount = 0;

    errorCount += loadSimulationConfig(fileOpen_Object,simulationConf);
    errorCount += loadPark(fileOpen_Object,park);

    if(errorCount > 0){

        printError("Could load configuration correctly!");
        printWarning("The program will exit!");
        exit(1);
    } else
        printInfo("Configuration Loaded");
}












