#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "registo.h"
#include "../common/events.h"
#include "../common/consoleAddons.h"
#include "socketClient.h"
#include "../common/socketUtils/socketComms.h"

extern int serverSocket;

int main(int argc , char *argv[] ){

    system("clear");

    printWarning("Monitor Started !");

    FILE* file = fopen("registo.txt", "w");
    if (file == NULL) {
        fprintf(stderr, "Failed to open the file for writing.\n");
        return 1; // Return an error code to indicate failure
    }

    // writeRecord(file, 0, SimulationStart, 1);
    // writeRecord(file, 1, UserJoined, 2);
    // writeRecord(file, 2, UserJoined, 3);
    // writeRecord(file, 2, UserLeft, 4);
    // writeRecord(file, 1, UserLeft, 5);
    // writeRecord(file, 0, SimulationStart, 6);

    creatConnection();

    char* str = NULL;
    int error;
    while(1){
        

        str = recvMsg(serverSocket,512,4);
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

// char* getSimulatorEventName( int event){

//     switch(event){
//         case(SIMULATION_STARTED):{
//             return "Simulation Started";
//         }
//         case(SIMULATION_ENDED):{
//             return "Simulation Ended";
//         }
//         case(SIMULATION_ERROR):{
//             return "Simulation Error";
//         }
//         case(SIMULATION_USER_CREATED):{
//             return "Simulation User Created";
//         }
//         case(SIMULATION_MESSAGE):{
//             return "Simulation Message";
//         }
//     }
// }

// char* getParkEventName( int event){

//     switch(event){
//         case(PARK_OPEN):{
//             return "Park Opened";
//         }
//         case(PARK_CLOSED):{
//             return "Park Closed";
//         }
//     }
// }

// char* getAttractionEventName( int event){

//     switch(event){
//         case(ATTRACTION_OPEN):{
//             return "Attraction Started";
//         }
//         case(ATTRACTION_CLOSED):{
//             return "Attraction Ended";
//         }
//         case(RIDE_STARTED):{
//             return "Attraction Ride Started";
//         }
//         case(RIDE_ENDED):{
//             return "Attraction Ride Ended";
//         }
//     }
// }

// char* getUserEventName( int event){

//     switch(event){
//         case(ENTERING_PARK):{
//             return "User Entered Park";
//         }
//         case(LEAVING_PARK):{
//             return "User Left Park";
//         }
//         case(ENTERING_WAITING_LINE):{
//             return "User Entered Waiting Line";
//         }
//         case(LEAVING_WAITING_LINE):{
//             return "User Left Waiting Line";
//         }
//         case(USING_VIP):{
//             return "User Used Vip";
//         }
//         case(ENTERING_RIDE):{
//             return "User Entered Ride";
//         }
//         case(LEAVING_RIDE):{
//             return "User Left Ride";
//         }
//         case(ENTERING_ATTRACTION):{
//             return "User Entered Attraction";
//         }
//         case(LEAVING_ATTRACTION):{
//             return "User Left Attraction";
//         }
//         case(ENTERING_DENIED):{
//             return "User Entering Denied";
//         }
//     }
// }