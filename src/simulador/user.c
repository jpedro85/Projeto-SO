#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#include "user.h"
#include "socketServer/socketServer.h"
#include "../common/consoleAddons.h"
#include "../common/events.h"
#include "../common/date.h"
#include "attraction.h"
#include "globals.h"

int id = 0;
pthread_t parkClientThread;

#define LEAVE_PARK 0
#define STAY_AT_PARK 1
#define LEAVE_ATTRACTION 0
#define STAY_AT_ATTRACTION 1
/**
 * The function creates park clients with random arrival times.
 */
void *createParkClients()
{
    printWarning("server: started creating users.");
    while (true)
    {
        sem_wait(&park.parkVacancy_sem_t);

        // Randomizes arrival time between the clients
        int userWaitingTime = rand() % (simulationConf.averageClientArriveTime_ms - simulationConf.toleranceClientArriveTime_ms) + simulationConf.toleranceClientArriveTime_ms;

        createParkClient(userWaitingTime);
    }
}

/**
 * The function creates a new client with random values after a specified waiting time.
 *
 * @param waitTime The waitTime parameter is the amount of time in milliseconds that the function
 * should wait before creating a new client.
 */
void createParkClient(int waitTime)
{
    // Converting waitTime thats in ms to microsecond as usleep works with that unit of time
    int waitTimeToMicrosecond = waitTime * 1000;
    // Simulating the waiting time with usleep function
    usleep(waitTimeToMicrosecond);

    // Creating the new Client with random values
    User *newUser = (User *)malloc(sizeof(User));
    createRandomClient(newUser);
    pthread_create(&parkClientThread, NULL, simulateUserActions, newUser);

    // Event userCreated = createEvent(SIMULATOR_EVENT,SIMULATION_USER_CREATED,getCurrentSimulationDate(startTime,simulationConf.dayLength_s));
    EvenInfo_SimulationUserCreated userInfo;
    userInfo.userId = newUser->id;
    userInfo.userAge = newUser->age;
    userInfo.hasVipPass = newUser->vipPass;
    asyncCreateEvent_UserCreated(getCurrentSimulationDate(startTime, simulationConf.dayLength_s), userInfo, 6, addMsgToQueue);

    // createEventInfoFor_SimulationUserCreated(&userCreated,userInfo);
    // async_addMsgToQueue(eventToJSON_String(userCreated,6));
}

/**
 * The function creates a random client by assigning a unique ID, generating a random age, and
 * determining if the client has a VIP pass.
 *
 * @param user A pointer to a User struct.
 */
void createRandomClient(User *user)
{
    user->id = id;
    id++;
    user->age = rand() % (simulationConf.userMaxAge - simulationConf.userMinAge) + simulationConf.userMinAge;
    // TODO alter probability to have vip pass depending on configuration file
    int hasVipPass = rand() % 100 < 20 ? 1 : 0;
    user->vipPass = hasVipPass;
    user->currentAttraction=-1;
}

/**
 * The function simulates user actions by either choosing an attraction or removing the client from the
 * park.
 *
 * @param client The "client" parameter is a pointer to a User object.
 */
void *simulateUserActions(void *client) {
    
    User *parsedClient = (User *)client;

    char formattedString[100];
    sprintf(formattedString, "\nThe client %d has entered the park", parsedClient->id);
    addMsgToQueue(formattedString);

    while (true)
    {
        // Randomizing the client action of whether wants to continue on the park or leave
        int clientChoice = chooseAction();

        // Checking the action that was choosen
        if (clientChoice == STAY_AT_PARK)
        {
            chooseAttraction(parsedClient);
        }
        else
        {
            removeClient(parsedClient);
            break;
        }
    }
}

/**
 * The function removes a client and signals that a parking space is available.
 */
void removeClient(User *client)
{
    char formattedString[100];
    sprintf(formattedString, "\nThe client %d has left the park", client->id);
    addMsgToQueue(formattedString);
    sem_post(&park.parkVacancy_sem_t);
    pthread_exit(0);
}

/**
 * The function chooseAction randomly decides whether to leave or stay at a park. Depending on the paramater defined on the config file
 *
 * @return either the value of the LEAVE_PARK or STAY_AT_PARK
 */
int chooseAction()
{
    //TODO: Change Probabilities of each case
    if(user.currentAttraction!=-1){
        return (((rand() % 100) + 1) <= 20) ? LEAVE_ATTRACTION : STAY_AT_ATTRACTION;
    }
    return (((rand() % 100) + 1) <= 5) ? LEAVE_PARK : STAY_AT_PARK;
}

void chooseAttraction(User *client) {
    // Making a random choice between 0 and the number of attractions to enter an attraction
    int parkAttractionsCount = park.attractions.length;
    int attractionChosenIndex = rand() % parkAttractionsCount;
    Attraction *attractionChosen = (Attraction *)getValueByIndex_LinkedList(&park.attractions, attractionChosenIndex);

    // If the client meets the requirements or not to enter the ride
    // If not it leaves the chooses another action
    if (!canClientBeOnAttraction(client, attractionChosen))
    {
        // TODO: Uncomment
        // EventInfo_UserEvent eventInfo;
        // eventInfo.clientID = client->id;
        // eventInfo.attractionName = attractionChosen->name;
        // asyncCreateEvent_UserEvent(getCurrentSimulationDate(startTime, simulationConf.dayLength_s), eventInfo, ENTERING_DENIED, sizeof(eventInfo), addMsgToQueue);
        
        return;
    }
    // After being on the first attraction
    if(client->currentAttraction!=-1){
        leaveAttraction(client, client->currentAttraction);
        enterAttraction(client, attractionChosen);
    }

    // Call the function to enter the attraction
    enterAttraction(client, attractionChosen);

    // Simulate the client's time on the attraction according to the config parameter
    usleep(attractionChosen->duration_ms * 1000);
}


bool canClientBeOnAttraction(User *client, Attraction *attraction)
{
    // checks for the age, if its open or not, if its running
    if (client->age < attraction->minAge || (attraction->maxAge != 0 && client->age > attraction->maxAge) || !attraction->isOpen)
    {
        return false;
    }
    return true;
}