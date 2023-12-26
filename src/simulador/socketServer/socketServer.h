#ifndef SOCKETSERVER_H
#define SOCKETSERVER_H
#define CLOSED 11

#include "../../common/socketUtils/socketComms.h"

void acceptClient();

void* waitForClients();

void waitFirstConnection();

void startServer();

void stopServer();

void* removeSendedMsgs();

void* sendMsgToClient( void* client );

void addMsgToQueue(char* msg);
void async_addMsgToQueue(char* msg);

#endif

