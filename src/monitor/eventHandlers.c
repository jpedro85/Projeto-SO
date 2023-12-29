#include <stdlib.h>

#include "../common/events.h"
#include "../common/consoleAddons.h"


void simulatorEvent_STARTED_handler(Event event){
    char* eventInString = eventToString(event,NULL);
    printWithColor(255,165,0,eventInString);

    //TODO:statistics and save in file

    free(eventInString);
}

void simulatorEvent_ENDED_handler(Event event){
    char* eventInString = eventToString(event,NULL);
    printWithColor(255,165,0,eventInString);

    //TODO:statistics and save in file
    free(eventInString);
}

void simulatorEvent_USER_CREATED_handler(Event event){
    char* eventInString = eventToString(event,extractEvent_SimulationUserCreated);
    printWithColor(255,165,0,eventInString);

    //TODO:statistics and save in file
    free(eventInString);
}

/**
 * The function `simulatorEvent_handler` handles different types of simulation events by calling the
 * corresponding event handler functions.
 * 
 * @param event The parameter "event" is of type "Event" 
 */
void simulatorEvents_handler(Event event){

    switch (event.event)
    {
        case SIMULATION_USER_CREATED:
            simulatorEvent_USER_CREATED_handler(event);
            break;
        case SIMULATION_STARTED:
            simulatorEvent_STARTED_handler(event);
            break;
        case SIMULATION_ENDED:
            simulatorEvent_ENDED_handler(event);
            break;
        case SIMULATION_ERROR:
            //TODO: erroMsg
            printWarning("not implemented yet1");
            break;
        case SIMULATION_MESSAGE:
            //TODO: simulatorMSG
            printWarning("not implemented yet2");
            break;
        default:
            printError("Wrong SIMULATOR_EVENT type.");
            break;
    }
    
}

void parkEvent_PARK_OPEN_handler(Event event){
    char* eventInString = eventToString(event,NULL);
    printWithColor(255,0,213,eventInString);

    //TODO:statistics and save in file

    free(eventInString);
}

void parkEvent_PARK_CLOSED_handler(Event event){
    char* eventInString = eventToString(event,NULL);
    printWithColor(255,0,213,eventInString);

    //TODO:statistics and save in file

    free(eventInString);
}

/**
 * The function "ParkEvent_handler" handles different types of park events by calling specific event
 * handlers based on the event type.
 * 
 * @param event The parameter "event" is of type "Event" and represents the event that occurred in the
 * park.
 */
void parkEvents_handler(Event event){
    
    switch (event.event)
    {
        case PARK_OPEN:
            parkEvent_PARK_OPEN_handler(event);
            break;
        case PARK_CLOSED:
            parkEvent_PARK_CLOSED_handler(event);
            break;
        default:
            printError("Wrong PARK_EVENT type.");
            break;
    }
}




void attractionEvent_ATTRACTION_OPEN_handler(Event event){
    char* eventInString = eventToString(event,extractEvent_AttractionEvent);
    printWithColor(200,0,0,eventInString);

    EventInfo_AttractionEvent eventInfo = getInfoEvent_AttractionEvent(&event);

    //TODO:statistics

    free(eventInString);
}

void attractionEvent_ATTRACTION_CLOSED_handler(Event event){
    char* eventInString = eventToString(event,extractEvent_AttractionEvent);
    printWithColor(200,0,0,eventInString);

    EventInfo_AttractionEvent eventInfo = getInfoEvent_AttractionEvent(&event);

    //TODO:statistics

    free(eventInString);
}

void attractionEvent_ATTRACTION_RIDE_STARTED_handler(Event event){
    char* eventInString = eventToString(event,extractEvent_AttractionRideEvent);
    printWithColor(200,0,100,eventInString);

    EventInfo_AttractionRideEvent eventInfo = getInfoEvent_AttractionRideEvent(&event);

    //TODO:statistics

    free(eventInString);
}

void attractionEvent_ATTRACTION_RIDE_ENDED_handler(Event event){
    char* eventInString = eventToString(event,extractEvent_AttractionRideEvent);
    printWithColor(200,0,100,eventInString);

    EventInfo_AttractionRideEvent eventInfo = getInfoEvent_AttractionRideEvent(&event);

    //TODO:statistics

    free(eventInString);
}

/**
 * The function "attractionEvents_handler" handles different types of attraction events by calling
 * specific event handlers based on the event type.
 * 
 * @param event The parameter "event" is of type "Event".
 */
void attractionEvents_handler(Event event){

    switch(event.event){
        case ATTRACTION_OPEN:
            attractionEvent_ATTRACTION_OPEN_handler(event);
            break;
        case ATTRACTION_CLOSED:
            attractionEvent_ATTRACTION_CLOSED_handler(event);
            break;
        case RIDE_STARTED:
            attractionEvent_ATTRACTION_RIDE_STARTED_handler(event);
            break;
         case RIDE_ENDED:
            attractionEvent_ATTRACTION_RIDE_ENDED_handler(event);
            break;
        default:
            printError("Wrong ATTRACTION_EVENT type.");
            break;
    }
}






void userEvent_ENTERING_PARK_handler(Event event){
    char* eventInString = eventToString(event,extractEvent_UserEventPark);
    printWithColor(255,255,255,eventInString);

    EventInfo_UserEventPark eventInfo = getInfoEvent_UserEventPark(&event);

    //TODO:statistics

    free(eventInString);
}

void userEvent_LEAVING_PARK_handler(Event event){
    char* eventInString = eventToString(event,extractEvent_UserEventPark);
    printWithColor(255,255,255,eventInString);

    EventInfo_UserEventPark eventInfo = getInfoEvent_UserEventPark(&event);

    //TODO:statistics

    free(eventInString);
}

void userEvent_ENTERING_WAITING_LINE_handler(Event event){
    char* eventInString = eventToString(event,extractEvent_UserEventWaitingLine);
    printWithColor(0,255,0,eventInString);

    EventInfo_UserEventWaitingLine eventInfo = getInfoEvent_UserEventWaitingLine(&event);

    //TODO:statistics

    free(eventInString);
}

void userEvent_LEAVING_WAITING_LINE_handler(Event event){
    char* eventInString = eventToString(event,extractEvent_UserEventWaitingLine);
    printWithColor(0,255,0,eventInString);

    EventInfo_UserEventWaitingLine eventInfo = getInfoEvent_UserEventWaitingLine(&event);

    //TODO:statistics

    free(eventInString);
}

void userEvent_ENTERING_RIDE_handler(Event event){
    char* eventInString = eventToString(event,extractEvent_UserEvent);
    printWithColor(0,0,255,eventInString);

    EventInfo_UserEvent eventInfo = getInfoEvent_UserEvent(&event);

    //TODO:statistics

    free(eventInString);
}

void userEvent_LEAVING_RIDE_handler(Event event){
    char* eventInString = eventToString(event,extractEvent_UserEvent);
    printWithColor(0,0,255,eventInString);

    EventInfo_UserEvent eventInfo = getInfoEvent_UserEvent(&event);

    //TODO:statistics

    free(eventInString);
}

void userEvent_LEAVING_ATTRACTION_handler(Event event){
    char* eventInString = eventToString(event,extractEvent_UserEvent);
    printWithColor(0,0,255,eventInString);

    EventInfo_UserEvent eventInfo = getInfoEvent_UserEvent(&event);

    //TODO:statistics

    free(eventInString);
}

void userEvent_USING_VIP_handler(Event event){
    char* eventInString = eventToString(event,extractEvent_UserEvent);
    printWithColor(0,150,150,eventInString);

    EventInfo_UserEvent eventInfo = getInfoEvent_UserEvent(&event);

    //TODO:statistics

    free(eventInString);
}

void userEvent_ENTERING_DENIED_handler(Event event){
    char* eventInString = eventToString(event,extractEvent_UserEvent);
    printWithColor(0,150,150,eventInString);

    EventInfo_UserEvent eventInfo = getInfoEvent_UserEvent(&event);

    //TODO:statistics

    free(eventInString);
}

/**
 * The function UserEvents_handler handles different events related to a park, waiting line, VIP
 * access, ride, and denied entry.
 * 
 * @param event The parameter "event" is of type "Event".
 */
void userEvents_handler(Event event){

     switch(event.event){
        case ENTERING_PARK:
            userEvent_ENTERING_PARK_handler(event);
            break;
        case LEAVING_PARK:
            userEvent_LEAVING_PARK_handler(event);
            break;
        case ENTERING_WAITING_LINE:
            userEvent_ENTERING_WAITING_LINE_handler(event);
            break;
        case LEAVING_WAITING_LINE:
            userEvent_LEAVING_WAITING_LINE_handler(event);
            break;
        case USING_VIP:
            userEvent_USING_VIP_handler(event);
            break;
        case ENTERING_RIDE:
            userEvent_ENTERING_RIDE_handler(event);
            break;
        case LEAVING_RIDE:
            userEvent_LEAVING_RIDE_handler(event);
            break;
        case LEAVING_ATTRACTION:
            userEvent_LEAVING_ATTRACTION_handler(event);
            break;
        case ENTERING_DENIED:
            userEvent_ENTERING_DENIED_handler(event);
            break;
    }
}