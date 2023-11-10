#ifndef USER_H
#define USER_H

#include <pthread.h>

typedef struct
{
    int id;
    int age;
    int vipPass;
    pthread_t currentAttraction; // Assuming pthread_t is the type for a thread
} User;


void* createParkClients();

#endif