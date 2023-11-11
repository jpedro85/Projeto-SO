#ifndef EVENTS_H
#define EVENTS_H

#include "cjson/cJSON.h"
#include "date.h"

typedef struct {
    int event;
    Date date;
    cJSON eventInfoJson;
}EventInfo;

typedef enum {
    SIMULATOR_EVENT,
    PARK_EVENT,
    ATTRACTION_EVENT,
    USER_EVENT,
}EventType;

typedef enum {
    SIMULATION_STARTED,
    SIMULATION_ENDED,
    SIMULATION_ERROR,
}SimulatorEvent;

typedef enum {
    PARK_OPEN,
    PARK_CLOSED,
}ParkEvent;

typedef enum {
    OPEN,
    CLOSED,
    RIDE_STARTED,
    RIDE_ENDED,
    ENTERING_DENIED,
}AttractionEvent;

typedef enum {
    ENTERING_PARK,
    LEAVING_PARK,
    ENTERING_WAITING_LINE,
    LEAVING_WAITING_LINE,
    USING_VIP,
    ENTERING_RIDE,
    LEAVING_RIDE,
    ENTERING_ATTRACTION,
    LEAVING_ATTRACTION,
}UserEvent;

// Functions to stringify an event
char* eventToJSON_String(EventType eventType, EventInfo eventInfo);

// Functions to specify event info for SimulatorEvent
cJSON* evenInfoToJson_SimulationStarted();
cJSON* evenInfoToJson_SimulationEnded();
cJSON* evenInfoToJson_SimulationError(); // esta 

// Functions to specify event info for ParkEvent 
cJSON* evenInfoToJson_ParkOpen(); // esta 
cJSON* evenInfoToJson_ParkClosed(); // esta 

// Functions to specify event info for AttractionEvent
cJSON* evenInfoToJson_AttractionOpen();  
cJSON* evenInfoToJson_AttractionClosed();
cJSON* evenInfoToJson_AttractionRideStarted();  
cJSON* evenInfoToJson_AttractionRideEnded();

// Functions to specify event info for AttractionEvent
cJSON* evenInfoToJson_UserEnteringPark();  
cJSON* evenInfoToJson_UserLeavingPark();
cJSON* evenInfoToJson_UserEnteringWaitingLine(/*bool vipAcess*/);  
cJSON* evenInfoToJson_UserLeavingWaitingLine();
cJSON* evenInfoToJson_UserEnteringRide();  
cJSON* evenInfoToJson_UserLeavingRide();
cJSON* evenInfoToJson_UserEnteringAttraction();  
cJSON* evenInfoToJson_UserLeavingAttraction();

char* eventMessage(int event);
#endif