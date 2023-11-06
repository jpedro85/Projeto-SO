#ifndef SOCKETSERVER_H
#define SOCKETSERVER_H
#define CLOSED 11

#include "../common/sokectUtils/socketComms.h"
#include <pthread.h>

void acceptClient();

void* waitForClients();

void waitFirstConnection();

void startServer();

void stopServer();

void* removeSendedMsgs();

void* sendMsgToClient( void* client );

void addMsgToQueue(char* msg);

typedef struct{
    
    pthread_t thread_recv;
     pthread_t thread_send;
    int socket;
    int index;

}Client;

#endif

