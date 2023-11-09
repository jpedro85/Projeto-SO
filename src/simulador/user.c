#include "user.h"
#include "globals.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <unistd.h>

int id = 0;
static bool vipPass;

Park park;
SimulationConf simulationConf;

void createRandomClient(User *user);

void *createParkClients()
{
    // TODO: Define max number clients (threads) outside of the park
    // * For now its in infinite while loop
    while (true)
    {
        // Randomizes arrival time between the clients
        int userWaitingTime = rand() % (simulationConf.averageClientArriveTime_ms - simulationConf.toleranceClientArriveTime_ms) + simulationConf.toleranceClientArriveTime_ms;
        printf("waiting Time: %d s",userWaitingTime);

        createClient(userWaitingTime);
    }
}

void createClient(int waitTime)
{
    // Converting waitTime thats in ms to microsecond as usleep works with that unit of time
    int witTimeToMicrosecond = waitTime * 1000;
    // Simulating the waiting time with usleep function
    usleep(witTimeToMicrosecond);

    // Creating the new Client with random values
    User newUser;
    createRandomClient(&newUser);
    printf("\n=========================================\n");
    printf("user id: %d\n", newUser.id);
    printf("user age: %d\n", newUser.age);
    printf("user has vip Pass: %d\n", newUser.vipPass);
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
    int hasVipPass= rand() %100 <20 ? 1 :0;
    user->vipPass = hasVipPass;
}

void *simulateUserActions()
{
}

void removeClient() {}

void chooseAction() {}

void chooseAttraction() {}
