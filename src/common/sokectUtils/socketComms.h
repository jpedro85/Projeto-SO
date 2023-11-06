#ifndef SOCKETCOMMS_H
#define SOCKETCOMMS_H

#define CONNECTION_CLOSED 10

typedef struct{
    char* msg;
    int numberOfSends;
    int id;
}Msg;

int sendMsg(int sendTo_fd, char* msg);
char* recvMsg(int recvFrom_fd, int bufferSize);

#endif