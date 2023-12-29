#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <errno.h>

#include "user.h"
#include "socketServer/socketServer.h"
#include "../common/mutexAddons.h"
#include "../common/consoleAddons.h"
#include "../common/events.h"
#include "../common/date.h"
#include "../common/linked_list.h"
#include "attraction.h"
#include "globals.h"

int id = 0;
pthread_t parkClientThread;

typedef enum{
    LEAVE_PARK,
    STAY_AT_PARK,
    LEAVE_ATTRACTION,
    STAY_AT_ATTRACTION,
    LEAVE_RIDE,
    STAY_AT_RIDE,
    WAIT_FOR_END_RIDE
}userActions;
/**
 * The function creates park clients with random arrival times.
 */
void *createParkClients()
{
    printWarning("server: started creating users.");
    while (true)
    {
        sem_wait(&(park.parkVacancy_sem_t));

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
    // TODO alter probability to have vip pass depending on configuration file (done)
    int hasVipPass = rand() % 100 < simulationConf.userHasVipPassChance_percentage ? 1 : 0;
    user->vipPass = hasVipPass;
    user->currentAttraction=NULL;
    user->state=IN_PARK;
}

/**
 * The function simulates user actions by either choosing an attraction or removing the client from the
 * park.
 *
 * @param client The "client" parameter is a pointer to a User object.
 */
int chooseAction(User* user);
void *simulateUserActions(void *client) {
    
    User *parsedClient = (User *)client;

    EventInfo_UserEventPark eventInfo;
    eventInfo.clientID = parsedClient->id;
    asyncCreateEvent_UserEventPark(getCurrentSimulationDate(startTime, simulationConf.dayLength_s), eventInfo, ENTERING_PARK, sizeof(eventInfo), addMsgToQueue);
    

    while (true)
    {
        // Randomizing the client action of whether wants to continue on the park or leave
        int clientChoice = chooseAction(parsedClient);
        printf("Client Choice: %d",clientChoice);
        //TODO: Perguntar ao ricardo se o usleep é suficiente para ele esperar um certo tempo para voltar a decidir se ele quer sair
        // Checking the action that was choosen
        if (clientChoice == STAY_AT_PARK){
            chooseAttraction(parsedClient);
        }
        else if(clientChoice == LEAVE_PARK){
            removeClient(parsedClient);
        }
        else if(clientChoice==STAY_AT_ATTRACTION){
            lockMutex(&(parsedClient->currentAttraction->waitingLine_mutex_t),"waitingLine_mutex_t");
            User* firstInLine =(User*) getValueByIndex_LInkedList(&(parsedClient->currentAttraction->waitingLine),0);
            unlockMutex(&(parsedClient->currentAttraction->waitingLine_mutex_t),"waitingLine_mutex_t");
            if(firstInLine->id==parsedClient->id){
                if(trySemWait(&(parsedClient->currentAttraction->enterRide_sem_t),"enterRide_sem_t")==0){
                    enterAttractionRide(parsedClient, parsedClient->currentAttraction);
                }
            }
        }
        
        else if (clientChoice==LEAVE_ATTRACTION){
            leaveAttraction(parsedClient, parsedClient->currentAttraction);
        }

        else if(clientChoice == STAY_AT_RIDE){
            printf("WHISLE!!!");
        }
        else if(clientChoice == LEAVE_RIDE){
            leaveAttraction(parsedClient, parsedClient->currentAttraction);
        }
        else if(clientChoice == WAIT_FOR_END_RIDE){
            int isRunning=0;
            do
            {
                isRunning=isAttractionRunning(parsedClient->currentAttraction);

            } while (isRunning==EBUSY || (isRunning==1));
            leaveAttraction(parsedClient, parsedClient->currentAttraction);
        }
        
        
    }
}

/**
 * The function removes a client and signals that a parking space is available.
 */
void removeClient(User *client)
{
    sem_post(&park.parkVacancy_sem_t);

    EventInfo_UserEventPark eventInfo;
    eventInfo.clientID = client->id;
    asyncCreateEvent_UserEventPark(getCurrentSimulationDate(startTime, simulationConf.dayLength_s), eventInfo, LEAVING_PARK, sizeof(eventInfo), addMsgToQueue);    

    pthread_exit(0);
}

/**
 * The function chooseAction randomly decides whether to leave or stay at a park. Depending on the paramater defined on the config file
 *
 * @return either the value of the LEAVE_PARK or STAY_AT_PARK
 */
int chooseAction(User *user)
{
    //TODO: Change Probabilities of each case. Use the conf values!!! (5=parkchance) (done)
    int parkIsOpen=0;
    readlock(&(park.parkIsOpen_rwlock_t),"parkIsOpen_rwlock_t");
    parkIsOpen=park.isOpen;
    rwlock_unlock(&(park.parkIsOpen_rwlock_t),"parkIsOpen_rwlock_t");
    
    int attractionIsOpen=-1;
    if(user->currentAttraction!=NULL){
        readlock(&(user->currentAttraction->isOpen_rwlock_t),"isOpen_rwlock_t");
        attractionIsOpen=user->currentAttraction->isOpen;
        rwlock_unlock(&(user->currentAttraction->isOpen_rwlock_t),"isOpen_rwlock_t");

        if(!attractionIsOpen){
            if(!parkIsOpen){
                leaveAttraction(user,user->currentAttraction);
            }
            else{
                return LEAVE_ATTRACTION;
            }
        }
    }
    if(!parkIsOpen){         
        removeClient(user);        
    }

    printf("Current State: %d",user->state);
    switch(user->state){
        case IN_RIDE:
            if(user->currentAttraction->duration_ms==0){
                return (((rand() % 100) + 1) <= 20) ? LEAVE_RIDE : STAY_AT_RIDE;
            }
            else{
                return WAIT_FOR_END_RIDE;
            }
        break;
        case IN_WAITING_LINE:
            return (((rand() % 100) + 1) <= 20) ? LEAVE_ATTRACTION : STAY_AT_ATTRACTION;
        break;
        case IN_PARK:
            return (((rand() % 100) + 1) <= simulationConf.userLeaveChance_percentage) ? LEAVE_PARK : STAY_AT_PARK;
        break;
    }
    // if(user->currentAttraction!=NULL){
    //     if(user->state==IN_RIDE){
    //         if(user->currentAttraction->duration_ms==0){
    //             return (((rand() % 100) + 1) <= 20) ? LEAVE_RIDE : STAY_AT_RIDE;
    //         }
    //         else{
    //             return WAIT_FOR_END_RIDE;
    //         }
    //     }
    //     return (((rand() % 100) + 1) <= 20) ? LEAVE_ATTRACTION : STAY_AT_ATTRACTION;
    // }    
    // return (((rand() % 100) + 1) <= simulationConf.userLeaveChance_percentage) ? LEAVE_PARK : STAY_AT_PARK;
}

void chooseAttraction(User *client) {
    // Making a random choice between 0 and the number of attractions to enter an attraction
    int parkAttractionsCount = park.attractions.length;
    int attractionChosenIndex = rand() % parkAttractionsCount;
    Attraction *attractionChosen = (Attraction *)getValueByIndex_LInkedList(&(park.attractions), attractionChosenIndex);

    // If the client meets the requirements or not to enter the ride
    // If not it leaves the chooses another action
    if (!canClientBeOnAttraction(client, attractionChosen))
    {
        
        EventInfo_UserEvent eventInfo;
        eventInfo.clientID = client->id;
        eventInfo.attractionName = attractionChosen->name;
        // Event event = createEvent(USER_EVENT, ENTERING_DENIED,getCurrentSimulationDate(startTime, simulationConf.dayLength_s));
        // createEventInfoFor_UserEvent(&(event),eventInfo);
        // addMsgToQueue(eventToJSON_String(event,10));
        asyncCreateEvent_UserEvent(getCurrentSimulationDate(startTime, simulationConf.dayLength_s), eventInfo, ENTERING_DENIED, sizeof(eventInfo), addMsgToQueue);
        sleep(20);
        return;
    }
    else{
        // Call the function to enter the attraction
        enterAttraction(client, attractionChosen);
        usleep(simulationConf.userMinWaitingTime_ms*1000);
    }
//TODO: Do a while waiting to be the first in the line or chance to leave the waitingLine (after the min waiting time). (supostamente done)
//TODO: After this, he can enter the attraction. (done)
//TODO: After he joined the attraction (passed the semaphore), he may enter the ride.(done)
//TODO: Make a While when he is in the attraction to see if the attraction is still running (use tryreadlock). Before while, do print("*Whisle*"). (pedir ajuda ao richy)
//TODO: To leave the attraction, change the user's state to IN_PARK. Always update user states, depending on where he is (done)
}


bool canClientBeOnAttraction(User *client, Attraction *attraction)
{
    // checks for the age, if its open or not, if its running
    if (client->age < attraction->minAge || (attraction->maxAge != 0 && client->age > attraction->maxAge) || !(attraction->isOpen))
    {
        return false;
    }
    return true;
}

void startClientSimulation(){
    create_DetachThread(createParkClients, NULL, "Create Park Clients");    
}