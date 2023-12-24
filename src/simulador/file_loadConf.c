#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "park.h"
#include "simulationConf.h"
#include "attraction.h"
#include "schedule.h"
#include "../common/file.h"
#include "../common/consoleAddons.h"

int loadItemLinkedListSchedule( cJSON* cJsonObject, char* objectName, LinkedList* linkedList ){

    printf("\033[1;33mLoading array %s.\033[1;0m\n",objectName); 
    cJSON* arraySchedule =  cJSON_GetObjectItem(cJsonObject,objectName);

    if(arraySchedule && cJSON_IsArray(arraySchedule)){

        cJSON* item;
        Schedule* schedule;
        int errorCount = 0;

        cJSON_ArrayForEach(item,arraySchedule){

            schedule = (Schedule*)malloc(sizeof(Schedule));

            errorCount += loadItemNumber(item,"startTime_ms",&(schedule->startTime_ms));
            errorCount += loadItemNumber(item,"endTime_ms",&(schedule->endTime_ms));

            if(errorCount > 0){
                printError("Could not load array");
                
                if(!isEmpty_LinkedList(linkedList))
                    clear_linkedList(linkedList);

                return 1;
            }else {
                addValue_LinkedList(linkedList,schedule);
            }

        }

        printWarning("Array Loaded");
        return 0;

    } else
        printf("\033[1;31mCan not load %s.\033[1;0m\n",objectName); 

    return 1;
}   

/**
 * The function loads attraction data from a JSON object into an Attraction struct.
 * 
 * @param attraction_Object attraction_Object is a pointer to a cJSON object that contains the data for
 * an attraction. This object should have the following attributes:
 * @param attraction Attraction is a struct that represents an attraction in a theme park. It contains
 * various attributes such as name, minimum age, maximum age, duration, ride capacity, schedule list,
 * current attendance, and waiting line. The loadAttraction function takes a cJSON object
 * (attraction_Object) and loads the
 * 
 * @return the number of errors encountered while loading the attraction data.
 */
int loadAttraction(cJSON* attraction_Object, Attraction* attraction){

    if(attraction_Object){

        int errorCount = 0;

        errorCount += loadItemString(attraction_Object,"name",&(attraction->name));
        errorCount += loadItemNumber(attraction_Object,"minAge",&(attraction->minAge));
        errorCount += loadItemNumber(attraction_Object,"maxAge",&(attraction->maxAge));
        errorCount += loadItemNumber(attraction_Object,"duration_ms",&(attraction->duration_ms));
        errorCount += loadItemNumber(attraction_Object,"rideCapacity",&(attraction->rideCapacity));
        errorCount += loadItemNumber(attraction_Object,"attractionRideMinLoad",&(attraction->attractionRideMinLoad));
        errorCount += loadItemNumber(attraction_Object,"rideBeginMaxWaitTime_ms",&(attraction->rideBeginMaxWaitTime_ms));

        initialize_LinkedList(&(attraction->scheduleList));
        errorCount += loadItemLinkedListSchedule(attraction_Object,"scheduleList",&(attraction->scheduleList));
        initialize_LinkedList(&(attraction->waitingLine));


        return errorCount;

    } else
        return 1;
}

/**
 * The function loads an array of attractions from a JSON object into a linked list.
 * 
 * @param cJsonObject A pointer to a cJSON object that contains the array of attractions.
 * @param objectName The name of the object that contains the array of attractions.
 * @param linkedList The `linkedList` parameter is a pointer to a `LinkedList` structure. This
 * structure is used to store a linked list of `Attraction` objects.
 * 
 * @return an integer value.
 */
int loadItemLinkedListAttraction( cJSON* cJsonObject, char* objectName, LinkedList* linkedList ){

    printf("\033[1;33mLoading array %s.\033[1;0m\n",objectName); 
    cJSON* arrayAttraction=  cJSON_GetObjectItem(cJsonObject,objectName);

    if(arrayAttraction && cJSON_IsArray(arrayAttraction)){

        cJSON* item;
        Attraction* attraction;
        int errorCount = 0;

        cJSON_ArrayForEach(item,arrayAttraction){

            printWarning("Loading Attraction.");

            attraction = (Attraction*)malloc(sizeof(Attraction));
            errorCount += loadAttraction(item,attraction);

            if(errorCount > 0){
                printError("Could not load array");

                if(!isEmpty_LinkedList(linkedList))
                    clear_linkedList(linkedList);

                return 1;
            }else{
                addValue_LinkedList(linkedList,attraction);
                printSuccess("Attraction Loaded correctly.");
            }

        }

        printWarning("Array Loaded.");
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
        errorCount += loadItemNumber(simulation,"numberOfDaysToSimulate",&(simulationConf->numberOfDaysToSimulate));
        errorCount += loadItemNumber(simulation,"userLeaveChance_percentage",&(simulationConf->userLeaveChance_percentage));
        errorCount += loadItemNumber(simulation,"userHasVipPassChance_percentage",&(simulationConf->userHasVipPassChance_percentage));

        if(errorCount > 0){
            printf("\033[1;33mSimulation object Loaded with %d errors!\033[1;0m\n",errorCount);
        }else{
            printSuccess("Loaded simulation object successfully.");
            simulationConf->dayLength_ms = simulationConf->dayLength_s * 1000;
            return 0;
        }

    }else
        printError("Cannot locate simulation object.");
    
    return 1;
}

int loadPark(cJSON* cJsonObject, Park* park){

    printWarning("Loading park object.");
    cJSON* cJson_park =  cJSON_GetObjectItem(cJsonObject,"park");
    int errorCount = 0;

    if(cJson_park){

        errorCount += loadItemString(cJson_park,"name",&(park->name));
        errorCount += loadItemNumber(cJson_park,"parkCapacity",&(park->parkCapacity));
        initialize_LinkedList(&(park->scheduleList));
        errorCount += loadItemLinkedListSchedule(cJson_park,"scheduleList",&(park->scheduleList));
        initialize_LinkedList(&(park->attractions));
        errorCount += loadItemLinkedListAttraction(cJson_park,"parkAttractions",&(park->attractions));

        if(errorCount > 0){
            printf("\033[1;33mPark object Loaded with %d errors!\033[1;0m\n",errorCount);
        }else{
            printSuccess("Loaded Park object successfully.");
            return 0;
        }

    }else
        printError("Cannot locate park object.");
    
    return 1;
}

void loadConfig(Park* park,SimulationConf* simulationConf, char* configFile){

    
    cJSON* fileOpen_Object = loadAndParseJSONFromFile(configFile);

    if(!fileOpen_Object){

        printError("The configuration could not be loaded.");
        printWarning("Using default simulator configuration.");

        fileOpen_Object = loadAndParseJSONFromFile("simuladorDefault.conf");

        if(!fileOpen_Object){
            printError("The default configuration could not be loaded.");
            printWarning("The program will exit!");
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
        printSuccess("Configuration Loaded.");
}