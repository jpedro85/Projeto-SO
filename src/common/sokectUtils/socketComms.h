#ifndef SOCKETCOMMS_H
#define SOCKETCOMMS_H

typedef struct{
    char* msg;
    int numberOfSends;
    int id;
}Msg;

int sendMsg(int sendTo_fd, char* msg);
int recvMsg(int recvFrom_fd, int bufferSize, char** result);

#endif