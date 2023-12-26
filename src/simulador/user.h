#ifndef USER_H
#define USER_H

#include <pthread.h>
#include <stdbool.h>
#include "attraction.h"

typedef struct
{
    int id;
    int age;
    int vipPass;
    int currentAttraction; // Assuming pthread_t is the type for a thread
} User;


void* createParkClients();
void createParkClient(int waitTime);
void createRandomClient(User *user);
void *simulateUserActions(void *client);
int chooseAction();
void chooseAttraction(User *client);
void removeClient(User *client);
bool canClientBeOnAttraction(User *client, Attraction *attraction);


#endif