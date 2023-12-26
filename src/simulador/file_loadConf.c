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
        printError("Could not load configuration correctly!");
        printWarning("The program will exit!");
        exit(1);
    } else
        printSuccess("Configuration Loaded.");
}





int verifySchedule(Schedule* schedule,SimulationConf* simulationConf){

    if( (schedule->startTime_ms >= 0 && schedule->startTime_ms <= simulationConf->dayLength_ms) 
     && 
     (schedule->endTime_ms >= schedule->startTime_ms && schedule->endTime_ms <= simulationConf->dayLength_ms))
    {   
        return 0;
    }else {
        return 1;
    } 
}

int verifyAttraction(Attraction* attraction,SimulationConf* simulationConf){

    int errorCount = 0;
    if(attraction->minAge < 0){ 
        printError("Attraction minAge can not be < 0.");
        errorCount++;
    }

    if(attraction->maxAge < attraction->minAge){ 
        printError("Attraction maxAge can not be < minAge.");
        errorCount++;
    }

    if(attraction->duration_ms < 0 || attraction->duration_ms > simulationConf->dayLength_ms){ 
        printError("Attraction duration_ms can not be < 0 and can not be > dayLength_ms.");
        errorCount++;
    }

    if(attraction->rideCapacity < 0 || attraction->attractionRideMinLoad < 0 || attraction->rideBeginMaxWaitTime_ms < 0 ){ 
        printError("Attraction values < 0");
        errorCount++;
    }

    if(attraction->rideBeginMaxWaitTime_ms > attraction->duration_ms ){
        printError("rideBeginMaxWaitTime_ms >  attraction->duration_ms ");
        errorCount++;
    }

    if(attraction->attractionRideMinLoad > attraction->rideCapacity){
        printError("Attraction attractionRideMinLoad > rideCapacity");
        errorCount++;
    }

    ListItem* item;
    ForEach_LinkedList((&(attraction->scheduleList)),item){
        
        if( verifySchedule((Schedule*)(item->value),simulationConf) > 0){
            printf("\033[1;31mInvalid schedule in attraction %s\033[1;0m\n",attraction->name);
            errorCount += 1;
        }
    }

    return errorCount;
}

int verifyPark(Park* park,SimulationConf* simulationConf){

    int errorCount = 0;
    if(park->parkCapacity < 0){
        printError("Park Capacity can not be < 0.");
        errorCount++;
    }
    
    if(park->scheduleList.length == 0){
        printError("Park as no schedules");
        errorCount++;
    }else{
        ListItem* item;
        ForEach_LinkedList((&(park->scheduleList)),item){
            if( verifySchedule((Schedule*)(item->value),simulationConf) > 0){
            printError("Invalid schedule in park");
            errorCount += 1;
        }
        }
    }

    if(park->attractions.length == 0){
        printError("Park as no attractions");
        errorCount++;
    }else{

        ListItem* item;
        ForEach_LinkedList((&(park->attractions)),item){
            errorCount += verifyAttraction((Attraction*)(item->value),simulationConf);
        }
        
    }

    return errorCount;
}

int verifySimulationConf(SimulationConf* simulationConf){

    int errorCount = 0;
    if(simulationConf->userMaxAge < 0 || simulationConf->userMinAge < 0){
        printError("Values of userMaxAge or userMinAge are invalid");
        errorCount++;
    }

    if(simulationConf->userMaxAge < simulationConf->userMinAge){
        printError("Value of userMaxAge < userMaxAge.");
        errorCount++;
    }

    if(simulationConf->dayLength_s <= 0){
        printError("Value of dayLength_s <= 0.");
        errorCount++;
    }

    if(simulationConf->numberOfDaysToSimulate <= 0){
        printError("Value of numberOfDaysToSimulate <= 0.");
        errorCount++;
    }

    if(simulationConf->averageClientArriveTime_ms <= 0){
        printError("Value of averageClientArriveTime_ms <= 0.");
        errorCount++;
    }

    if(simulationConf->toleranceClientArriveTime_ms <= 0){
        printError("Value of averageClientArriveTime_ms <= 0.");
        errorCount++;
    }

    if(simulationConf->userMinWaitingTime_ms < 0 || simulationConf->userLeaveChance_percentage < 0 || simulationConf->userMinWaitingTime_ms < 0){
        printError("One or more userSetings are < 0.");
        errorCount++;
    }

    return errorCount;
}

/**
 * The function verifies the loaded values of a park and simulation configuration.
 * 
 * @param park A pointer to a Park struct, which contains information about the park.
 * @param simulationConf A pointer to a structure that contains the configuration settings for the
 * simulation.
 * 
 * @return an integer value.
 */
int verifyLoadedValues(Park* park,SimulationConf* simulationConf){

    int error = 0;
    error = verifySimulationConf(simulationConf);
    if(error > 0) return error;

    error += verifyPark(park,simulationConf);

    return error;
}