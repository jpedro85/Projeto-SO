#ifndef USER_H
#define USER_H

#include <pthread.h>
#include <stdbool.h>
#include <pthread.h>
#include "attraction.h"

typedef struct Attraction Attraction;
typedef struct User
{
    int id;
    int age;
    int vipPass;
    Attraction* currentAttraction;
    int state;
    int dayCreated;
    pthread_t userThread;
} User;

typedef enum{
    IN_RIDE,
    IN_WAITING_LINE,
    IN_PARK
}UserState;


void* createParkClients();
void createParkClient();
void createRandomClient(User *user);
void *simulateUserActions(void *client);
void chooseAttraction(User *client);
void removeClient(User *client);
bool canClientBeOnAttraction(User *client, Attraction *attraction);
void startClientSimulation();


#endif