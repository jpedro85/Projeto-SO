#include "user.h"
#include "socketServer/socketServer.h"
#include "attraction.h"
#include "globals.h"

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

int id = 0;
pthread_t parkClientThread;

#define LEAVE_PARK 0
#define STAY_AT_PARK 1

/**
 * The function creates park clients with random arrival times.
 */
void *createParkClients()
{
    while (true)
    {
        sem_wait(&park.parkVacancy);

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
    User newUser;
    createRandomClient(&newUser);
    pthread_create(&parkClientThread, NULL, simulateUserActions, &newUser);

    // printf("\n=========================================\n");
    // printf("user id: %d\n", newUser.id);
    // printf("user age: %d\n", newUser.age);
    // printf("user has vip Pass: %d\n", newUser.vipPass);
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
}

/**
 * The function simulates user actions by either choosing an attraction or removing the client from the
 * park.
 *
 * @param client The "client" parameter is a pointer to a User object.
 */
void *simulateUserActions(void *client)
{
    char formattedString[100];
    sprintf(formattedString, "\nThe client %d has entered the park", ((User *)client)->id);
    addMsgToQueue(formattedString);
    while (true)
    {
        User *parsedClient = (User *)client;

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
    sem_post(&park.parkVacancy);
    pthread_exit(0);
}

/**
 * The function chooseAction randomly decides whether to leave or stay at a park. Depending on the paramater defined on the config file
 *
 * @return either the value of the LEAVE_PARK or STAY_AT_PARK
 */
int chooseAction()
{
    return (((rand() % 100) + 1) <= 5) ? LEAVE_PARK : STAY_AT_PARK;
}

void chooseAttraction(User *client)
{
    // Making a random choice between 0 and the number of attractions to enter an attraction
    int parkAttractionsCount = park.attractions.length;
    int attractionChosenIndex = rand() % parkAttractionsCount;
    Attraction *attractionChosen = (Attraction *)getValueByIndex_LInkedList(&park.attractions, attractionChosenIndex);

    // If the client meets the requirements or not to enter the ride
    // If not it leaves the chooses another action
    if (!canClientBeOnAttraction(client, attractionChosen))
    {
        char formattedString[100];
        sprintf(formattedString, "\nThe client %d is not permitted to enter the attraction %s", client->id, attractionChosen->name);
        addMsgToQueue(formattedString);

        return;
    }

    // ? Maybe this part of the code is in attraction.c as it doesn't have anything to do with user in itself
    // TODO semaphore for the client to enter the attraction according to its capacity and current attendance

    // Simulates the client time on the attraction according to the config parameter
    usleep(attractionChosen->duration_ms * 1000);
}

bool canClientBeOnAttraction(User *client, Attraction *attraction)
{
    // TODO checks for the age, if its open or not, if its running
    if (client->age < attraction->minAge || (attraction->maxAge != 0 && client->age > attraction->maxAge) || !attraction->isOpen)
    {
        return false;
    }
    return true;
}