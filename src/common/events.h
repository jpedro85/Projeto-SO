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

// TODO: Review
typedef enum {
    ATTRACTION_OPEN,
    ATTRACTION_CLOSED,
    RIDE_STARTED,
    RIDE_ENDED,
}AttractionEvent;
extern char* attractionEventNames[];

// TODO: review
typedef enum {
    ENTERING_PARK, // clientID
    LEAVING_PARK, // clientID
    ENTERING_WAITING_LINE, // clientID, attractionName and line size
    LEAVING_WAITING_LINE, // clientID, attractionName and line Size
    USING_VIP, // clientID and attractionName
    ENTERING_RIDE, // clientID and attractionName
    LEAVING_RIDE, // clientID and attractionName 
    ENTERING_DENIED, // clientID and attractionName
    LEAVING_ATTRACTION, // clientID and attractionName
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

void asyncCreateEvent_WithoutInfo(Date date,EventType eventType, int eventEnumValue,EventMsgHandler handler);
typedef struct {
    int errorValue;
    char* errorMsg;
}EvenInfo_SimulationError;

void createEventInfoFor_SimulationError(Event* event, EvenInfo_SimulationError info); 
EvenInfo_SimulationError getInfoEvent_SimulationError(Event* event);
void asyncCreateEvent_SimulationError(Date date,EvenInfo_SimulationError eventInfo,int eventInfo_estimatedSize,EventMsgHandler handler);
char* extractEvent_SimulationError(Event* event);

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
char* extractEvent_SimulationMessage(Event* event);

typedef struct {
    char* attractionName;
} EventInfo_AttractionEvent;

void createEventInfoFor_AttractionEvent(Event* event, EventInfo_AttractionEvent info);
EventInfo_AttractionEvent getInfoEvent_AttractionEvent(Event* event);
void asyncCreateEvent_AttractionEvent(Date date, EventInfo_AttractionEvent eventInfo,int attractionEvent,int eventInfo_estimatedSize, EventMsgHandler handler);
char* extractEvent_AttractionEvent(Event* event);

typedef struct {
    char* attractionName;
    int usersInRide;
    int usersWaiting;
    int rideNumber;
} EventInfo_AttractionRideEvent;

void createEventInfoFor_AttractionRideEvent(Event* event, EventInfo_AttractionRideEvent info);
EventInfo_AttractionRideEvent getInfoEvent_AttractionRideEvent(Event* event);
void asyncCreateEvent_AttractionRideEvent(Date date, EventInfo_AttractionRideEvent eventInfo,int attractionEvent,int eventInfo_estimatedSize, EventMsgHandler handler);
char* extractEvent_AttractionRideEvent(Event* event);

typedef struct {
    int clientID;
    char* attractionName;
} EventInfo_UserEvent;

typedef struct {
    int clientID;
} EventInfo_UserEventPark;

typedef struct {
    int clientID;
    char* attractionName;
    int lineSize;
} EventInfo_UserEventWaitingLine;

void createEventInfoFor_UserEvent(Event* event, EventInfo_UserEvent info);
void createEventInfoFor_UserEventPark(Event* event, EventInfo_UserEventPark info);
void createEventInfoFor_UserEventWaitingLine(Event *event, EventInfo_UserEventWaitingLine info);
EventInfo_UserEvent getInfoEvent_UserEvent(Event* event);
EventInfo_UserEventPark getInfoEvent_UserEventPark(Event *event);
EventInfo_UserEventWaitingLine getInfoEvent_UserEventWaitingLine(Event *event);
void asyncCreateEvent_UserEvent(Date date, EventInfo_UserEvent eventInfo,int userEvent,int eventInfo_estimatedSize, EventMsgHandler handler);
void asyncCreateEvent_UserEventPark(Date date, EventInfo_UserEventPark eventInfo,int userEvent,int eventInfo_estimatedSize, EventMsgHandler handler);
void asyncCreateEvent_UserEventWaitingLine(Date date, EventInfo_UserEventWaitingLine eventInfo,int userEventWaitingLine,int eventInfo_estimatedSize, EventMsgHandler handler);
char* extractEvent_UserEvent(Event* event);
char* extractEvent_UserEventPark(Event* event);
char* extractEvent_UserEventWaitingLine(Event* event);


#endif