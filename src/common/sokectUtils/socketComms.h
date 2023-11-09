#ifndef SOCKETCOMMS_H
#define SOCKETCOMMS_H
#define CONNECTION_CLOSED 10

#include "../linked_list.h"

typedef struct{
    char* msg;
    LinkedList sendedTo;
    int id;
}Msg;

int sendMsg(int sendTo_fd, char* msg);
char* recvMsg(int recvFrom_fd, int bufferSize);

void* freeMsgValues(void* value);

#endif