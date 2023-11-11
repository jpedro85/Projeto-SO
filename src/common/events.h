#ifndef EVENTS_H
#define EVENTS_H

#include "cjson/cJSON.h"
#include "date.h"
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
}AttractionEvent;

typedef enum {
    ENTERING_PARK,
    LEAVING_PARK,
    ENTERING_WAITING_LINE,
    LEAVING_WAITING_LINE,
    ENTERING_RIDE,
    LEAVING_RIDE,
    ENTERING_ATTRACTION,
    LEAVING_ATTRACTION,
}UserEvent;

// Functions to stringify an event
char* SimulatorEventToJSON_String(SimulatorEvent simulationEvent, User user, Date date, cJSON* simulationEventInfo);
char* ParkEventToJSON_String(ParkEvent parkEvent, Date date, cJSON* parkEventInfo);
char* AttractionToJSON_String(AttractionEvent userEvent, Date date, cJSON* attractionEventInfo);
char* UserEventToJSON_String(UserEvent userEvent, Date date, cJSON* userEventInfo);

// Functions to specify event info for SimulatorEvent
cJSON* EvenInfoToJson_SimulationStarted();
cJSON* EvenInfoToJson_SimulationEnded();
cJSON* EvenInfoToJson_SimulationError(); // esta 

// Functions to specify event info for ParkEvent 
cJSON* EvenInfoToJson_ParkOpen(); // esta 
cJSON* EvenInfoToJson_ParkClosed(); // esta 

// Functions to specify event info for AttractionEvent
cJSON* EvenInfoToJson_AttractionOpen();  
cJSON* EvenInfoToJson_AttractionClosed();
cJSON* EvenInfoToJson_AttractionRideStarted();  
cJSON* EvenInfoToJson_AttractionRideEnded();

// Functions to specify event info for AttractionEvent
cJSON* EvenInfoToJson_UserEnteringPark();  
cJSON* EvenInfoToJson_UserLeavingPark();
cJSON* EvenInfoToJson_UserEnteringWaitingLine();  
cJSON* EvenInfoToJson_UserLeavingWaitingLine();
cJSON* EvenInfoToJson_UserEnteringRide();  
cJSON* EvenInfoToJson_UserLeavingRide();
cJSON* EvenInfoToJson_UserEnteringAttraction();  
cJSON* EvenInfoToJson_UserLeavingAttraction();





char* eventMessage(int event);

#endif