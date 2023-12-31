#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "registo.h"
#include "../common/consoleAddons.h"
#include "socketClient.h"
#include "../common/socketUtils/socketComms.h"
#include "../common/string_utils.h"

#include "eventHandllers.h"

extern int serverSocket;

void simulatorEvent_handler(Event event);

int main(int argc , char *argv[] ){

    system("clear");

    printWarning("Monitor Started !");

    FILE *file = fopen("relatorio.txt", "wt");
    if (file == NULL) {
        fprintf(stderr, "Failed to open the file for writing.\n");
        return 1; // Return an error code to indicate failure
    }
    fclose(file);

    creatConnection();

    char* str = NULL;
    int error;
    while(1){
        
        str = recvMsg(serverSocket,512,4);
        if(str == NULL){

            if(errno == CONNECTION_CLOSED)
                break;
            else
                printError(strerror(errno));

        } else {
            
            if(strContain_Substring(str,"{") > 0 ){
                Event event =  stringJsonTo_Event(str);
                switch (event.eventType)
                {
                    case SIMULATOR_EVENT:
                        simulatorEvents_handler(event);
                        break;
                    case PARK_EVENT:
                        parkEvents_handler(event);
                        break;
                    case ATTRACTION_EVENT:
                        attractionEvents_handler(event);
                        break;
                    case USER_EVENT:
                        userEvents_handler(event);
                        break;
                    default:
                        printError("Not Implemented YET");
                        break;
                }

                printf("\n");
            }else{
                 printWarning("NOT Reading an Event");
                printWarning(str);
                // free(str);
            }
        }
    }

    fclose(file); // Close the file

    closeConnection();
    printSuccess("Monitor terminated successfully");
    return 0;
}