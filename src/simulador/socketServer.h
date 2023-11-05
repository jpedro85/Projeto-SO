#ifndef SOCKETSERVER_H
#define SOCKETSERVER_H

#include "../common/sokectUtils/socketComms.h"
#include <pthread.h>

void acceptClient();

void* waitForClients();

void waitFirstConnection();

void waitFor_waitForClients(int connectionIndex);

void startServer();

void stopServer();

void* removeSendedMsgs();

void* sendMsgToClient( void* client );

void addMsgToQueue(char* msg);

typedef struct{
    
    pthread_t thread;
    int socket;

}Client;

#endif

