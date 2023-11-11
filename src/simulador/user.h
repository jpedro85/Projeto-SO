#ifndef USER_H
#define USER_H

#include <pthread.h>
#include <stdbool.h>

typedef struct
{
    int id;
    int age;
    int vipPass;
    pthread_t currentAttraction; // Assuming pthread_t is the type for a thread
} User;


void* createParkClients();
void createClient(int waitTime);
void createRandomClient(User *user);
void *simulateUserActions(void *client);
int chooseAction();
void chooseAttraction();
void removeClient(User *client);
bool canClientBeOnAttraction();


#endif