#ifndef EVENTS_H
#define EVENTS_H

#include "cjson/cJSON.h"
#include "date.h"

typedef struct {
    int eventType;
    int event;
    Date date;
    cJSON* eventInfoJson;
}Event;

typedef enum {
    SIMULATOR_EVENT,
    PARK_EVENT,
    ATTRACTION_EVENT,
    USER_EVENT,
}EventType;

typedef enum {
    PARK_OPEN,
    PARK_CLOSED,
}ParkEvent;
extern char* parkEventNames[];

// TODO: implement all
typedef enum {
    ATTRACTION_OPEN,
    ATTRACTION_CLOSED,
    RIDE_STARTED,
    RIDE_ENDED,
}AttractionEvent;
extern char* attractionEventNames[];

// TODO: Implement all this events
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
    ENTERING_DENIED,
}UserEvent;
extern char* userEventNames[];

// Function to stringify an event
char* eventToJSON_String(Event event ,int eventInfo_estimatedSize);
// Functions to parse string to Event
Event stringJsonTo_Event(char* stringJson);
// General Event
Event createEvent(EventType eventType, int event, Date eventDate);
// event to string
typedef char* (*eventInfoToString)(Event* event);
char* eventToString(Event event, eventInfoToString func);
char* getEventName(EventType type , int event);

// Function Pointer to handler the result of asyncCreateEvent
typedef void (*EventMsgHandler)(char*);

typedef enum {
    SIMULATION_STARTED,// general event without info
    SIMULATION_ENDED,// general event without info
    SIMULATION_ERROR,
    SIMULATION_USER_CREATED,
    SIMULATION_MESSAGE,
}SimulatorEvent;
extern char* simulatorEventNames[];

// Functions to specify event info for SimulatorEvent
typedef struct {
    int errorValue;
    char* errorMsg;
}EvenInfo_SimulationError;

void createEventInfoFor_SimulationError(Event* event, EvenInfo_SimulationError info); 
EvenInfo_SimulationError getInfoEvent_SimulationError(Event* event);
void asyncCreateEvent_SimulationError(Date date,EvenInfo_SimulationError eventInfo,int eventInfo_estimatedSize,EventMsgHandler handler);

typedef struct {
    int userId;
    int userAge;
    int hasVipPass;
}EvenInfo_SimulationUserCreated;

void createEventInfoFor_SimulationUserCreated(Event* event, EvenInfo_SimulationUserCreated info);
EvenInfo_SimulationUserCreated getInfoEvent_SimulationUserCreated(Event* event);
void asyncCreateEvent_UserCreated(Date date,EvenInfo_SimulationUserCreated eventInfo,int eventInfo_estimatedSize,EventMsgHandler handler);
char* extractEvent_SimulationUserCreated(Event* event);

typedef struct {
    char* msg;    
}EvenInfo_SimulationMessage;

void createEventInfoFor_SimulationMessage(Event* event, EvenInfo_SimulationMessage info);
EvenInfo_SimulationMessage getInfoEvent_SimulationMessage(Event* event);
void asyncCreateEvent_SimulationMessage(Date date, EvenInfo_SimulationMessage eventInfo,int eventInfo_estimatedSize, EventMsgHandler handler);

typedef struct {
    char* attractionName;
} EventInfo_AttractionEvent;

void createEventInfoFor_AttractionEvent(Event* event, EventInfo_AttractionEvent info);
EventInfo_AttractionEvent getInfoEvent_AttractionEvent(Event* event);
void asyncCreateEvent_AttractionEvent(Date date, EventInfo_AttractionEvent eventInfo,int eventInfo_estimatedSize, EventMsgHandler handler);

#endif