#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "registo.h"
#include "../common/events.h"
#include "../common/consoleAddons.h"
#include "socketClient.h"
#include "../common/sokectUtils/socketComms.h"

extern int serverSocket;

int main(int argc , char *argv[] ){

    printWarning("Monitor Started !");

    FILE* file = fopen("registo.txt", "w");
    if (file == NULL) {
        fprintf(stderr, "Failed to open the file for writing.\n");
        return 1; // Return an error code to indicate failure
    }

    writeRecord(file, 0, SimulationStart, 1);
    writeRecord(file, 1, UserJoined, 2);
    writeRecord(file, 2, UserJoined, 3);
    writeRecord(file, 2, UserLeft, 4);
    writeRecord(file, 1, UserLeft, 5);
    writeRecord(file, 0, SimulationStart, 6);

    creatConnection();

    char* str = NULL;
    int error;
    while(1){
        

        str = recvMsg(serverSocket,512);
        if(str == NULL)
            if(errno == CONNECTION_CLOSED)
                break;
            else
                printError(strerror(errno));
        else{
            printSuccess(str);
        }
    }

    fclose(file); // Close the file

    closeConnection();
    printSuccess("Monitor terminated successfully");
    return 0;
}