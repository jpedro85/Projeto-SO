#ifndef CLIENT_H
#define CLIENT_H

#include <pthread.h>

typedef struct{
    
    pthread_t thread_recv;
    pthread_t thread_send;
    int socket;
    int index;

}Client;

void* freeClientValues(void* value);

#endif