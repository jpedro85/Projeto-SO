#include <string.h>
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

#include "events.h"
#include "consoleAddons.h"
#include "file.h"
#include "string_utils.h"

char* parkEventNames[] = {
    "Park Opened",
    "Park Closed"
};

char* attractionEventNames[] = { 
    "Attraction Started",
    "Attraction Ended",
    "Attraction Ride Started",
    "Attraction Ride Ended" 
};

char* userEventNames[] = { 
    "User Entered Park",
    "User Left Park",
    "User Entered Waiting Line",
    "User Left Waiting Line",
    "User Used Vip",
    "User Entered Ride", 
    "User Left Ride",
    "User Entered Attraction",
    "User Left Attraction",
    "User Entering Denied"
};

char* simulatorEventNames[] = { 
    "Simulation Started",
    "Simulation Ended",
    "Simulation Error",
    "Simulation User Created",
    "Simulation Message"
};

/**
 * The function `stringJsonTo_Event` parses a JSON string into an `Event` struct.
 * 
 * @param stringJson A string containing a JSON representation of an event.
 * 
 * @return an Event object.
 */
Event stringJsonTo_Event(char* stringJson){

    Event event;
    int error = 0;
    cJSON* eventJson = cJSON_Parse(stringJson);
    if(!eventJson)
        error++;
    
    error += loadItemNumber2(eventJson,"eventType",&(event.eventType));
    error += loadItemNumber2(eventJson,"event",&(event.event));
    error += loadItemNumber2(eventJson,"day",&(event.date.day));
    error += loadItemNumber2(eventJson,"hour",&(event.date.hour));
    error += loadItemNumber2(eventJson,"minute",&(event.date.minute));
    event.eventInfoJson = cJSON_GetObjectItem(eventJson,"eventInfo");

    if(error > 0)
        printFatalError("Can not parse EventString to cJSON.");

    return event;
}


/**
 * The function eventToJSON_String takes in an eventType, eventInfo, and eventInfo_estimatedSize and
 * returns a JSON string representation of the event.
 * 
 * @param eventType The eventType parameter is of type EventType, which is an enumeration representing
 * different types of events. It specifies the type of the event being converted to JSON.
 * @param eventInfo The eventInfo parameter is a struct that contains information about the event.
 * @param eventInfo_estimatedSize The parameter `eventInfo_estimatedSize` is an integer that represents
 * the estimated size of the `eventInfo` object in bytes.
 * 
 * @return a JSON string representation of the event object.
 */
char* eventToJSON_String(Event event ,int eventInfo_estimatedSize){

    cJSON* eventJson = cJSON_CreateObject();
    cJSON_AddNumberToObject(eventJson,"eventType",event.eventType);
    cJSON_AddNumberToObject(eventJson,"event",event.event);
    cJSON_AddNumberToObject(eventJson,"day",event.date.day);
    cJSON_AddNumberToObject(eventJson,"hour",event.date.hour);
    cJSON_AddNumberToObject(eventJson,"minute",event.date.minute);

    if(event.eventInfoJson){
        // TODO: remove warning after all events are tested
        printWarning("Has EventInfo");
        cJSON_AddItemToObject(eventJson,"eventInfo",event.eventInfoJson);
    }

    return cJSON_PrintBuffered(eventJson,50 + eventInfo_estimatedSize,0);
}


/**
 * The function creates an event object with the specified event type, event enum value, and event
 * date.
 * 
 * @param eventType The eventType parameter is of type EventType, which is an enumeration that
 * represents different types of events. It could be something like EventType.CONCERT,
 * EventType.SPORTS, EventType.CONFERENCE, etc.
 * @param eventEnumValue The eventEnumValue parameter is an integer value that represents the specific
 * event within the given eventType. It is used to differentiate between different events of the same
 * type.
 * @param eventDate The eventDate parameter is of type Date and represents the date of the event.
 * 
 * @return an Event object.
 */
Event createEvent(EventType eventType, int eventEnumValue, Date eventDate){

    Event event;
    event.eventType = eventType;
    event.event = eventEnumValue;
    event.date = eventDate;
    event.eventInfoJson = NULL;
    return event;
}


typedef struct {
    Event* event;
    void* eventInfo;
}CreateEventInfo_Params;

typedef void(*CreateEventInfo)(CreateEventInfo_Params);
//typedef void(*EventMsgHandler)(char*);
typedef struct {

    EventType eventType;
    int eventEnumValue;
    Date eventDate;
    void* eventInfo;
    CreateEventInfo function_CreateEventInfo;
    EventMsgHandler function_EventMsgHandler;
    int EstimatedSize;

}CreateEvent_AsyncParam;

/**
 * The function creates and initializes a CreateEvent_AsyncParam struct with the given parameters.
 * 
 * @param eventType The type of event being created. It could be an enum value or a string representing
 * the event type.
 * @param eventEnumValue eventEnumValue 
 * @param date The "date" parameter is of type "Date" and represents the date of the event.
 * @param function_CreateEventInfo A function pointer that points to a function that takes one CreateEventInfo_Params.
 * @param eventInfo_estimatedSize The eventInfo_estimatedSize parameter is an integer that represents
 * the estimated size of the event information. 
 * @param function_EventMsgHandler function_EventMsgHandler is a function pointer that points to a
 * function that handles eventToString message.
 * 
 * @return a pointer to a structure of type CreateEvent_AsyncParam.
 */
CreateEvent_AsyncParam* create_CreateEvent_AsyncParam(EventType eventType,int eventEnumValue, Date date, CreateEventInfo function_CreateEventInfo, int eventInfo_estimatedSize , EventMsgHandler function_EventMsgHandler ){

    CreateEvent_AsyncParam* parameters = (CreateEvent_AsyncParam*)malloc(sizeof(CreateEvent_AsyncParam));
    parameters->eventType = eventType;
    parameters->eventEnumValue = eventEnumValue;
    parameters->eventDate = date;
    parameters->function_CreateEventInfo = function_CreateEventInfo;
    parameters->function_EventMsgHandler = function_EventMsgHandler;
    parameters->EstimatedSize = eventInfo_estimatedSize;

    return parameters;
}

typedef void* (*ThreadFunction) (void*);
void createDetachThread(ThreadFunction func , CreateEvent_AsyncParam* parameters){

    pthread_t thread;
    pthread_attr_t detachedThread;
    pthread_attr_init(&detachedThread);
    pthread_attr_setdetachstate(&detachedThread,PTHREAD_CREATE_DETACHED);

    if( pthread_create( &thread, &detachedThread, func , parameters) ){
        printFatalError("Can not create thread for async_CreateEvent.");
    }

    pthread_attr_destroy(&detachedThread);
}

void* asyncCreateEvent( void* createEvent_AsyncParam){

    CreateEvent_AsyncParam* param_creatEvent = (CreateEvent_AsyncParam*)createEvent_AsyncParam;
    Event event = createEvent( param_creatEvent->eventType, param_creatEvent->eventEnumValue, param_creatEvent->eventDate );
    
    if(param_creatEvent->eventInfo){

        CreateEventInfo_Params param;
        param.event = &event;
        param.eventInfo = param_creatEvent->eventInfo;

        param_creatEvent->function_CreateEventInfo(param);
        free(param.eventInfo);
        // TODO: remove warning after all events are tested
        printWarning("Freeing EventInfo");
    }

    param_creatEvent->function_EventMsgHandler( eventToJSON_String( event, param_creatEvent->EstimatedSize) );

    free(param_creatEvent);
}

/**
 * The function creates event information for a simulation error.
 * 
 * @param event The "event" parameter is a pointer to an Event structure. This structure 
 * contains information about an event.
 * @param info The parameter "info" is of type EvenInfo_SimulationError.
 */
void createEventInfoFor_SimulationError(Event* event, EvenInfo_SimulationError info){

    event->eventInfoJson = cJSON_CreateObject();
    cJSON_AddStringToObject(event->eventInfoJson,"errorMsg",info.errorMsg);
    cJSON_AddNumberToObject(event->eventInfoJson,"errnoVar",info.errorValue);
}
void general_createEventInfoFor_SimulationError(CreateEventInfo_Params params){
    createEventInfoFor_SimulationError(params.event, *((EvenInfo_SimulationError*)(params.eventInfo)));
}

void asyncCreateEvent_SimulationError(Date date,EvenInfo_SimulationError eventInfo,int eventInfo_estimatedSize,EventMsgHandler handler){
    
    CreateEvent_AsyncParam* parameters = create_CreateEvent_AsyncParam(
                                            SIMULATOR_EVENT,
                                            SIMULATION_ERROR,
                                            date,
                                            general_createEventInfoFor_SimulationError,
                                            eventInfo_estimatedSize,
                                            handler
                                         );

    EvenInfo_SimulationError* a = (EvenInfo_SimulationError*)malloc(sizeof(EvenInfo_SimulationError));
    *a = eventInfo;
    parameters->eventInfo = a;
    createDetachThread(asyncCreateEvent,parameters);
}

/**
 * The function `getInfoEvent_SimulationError` extracts information from a JSON object of 
 * and returns it as an `EvenInfo_SimulationError` object.
 * 
 * @param event The parameter "event" is a pointer to an Event object.
 * 
 * @return an object of type EvenInfo_SimulationError.
 */
EvenInfo_SimulationError getInfoEvent_SimulationError(Event* event){

    if(!event->eventInfoJson)
        printFatalError("Wrong event type not SimulationError, eventInfoJson is not defined.");

    EvenInfo_SimulationError eventInfo;
    int error = loadItemNumber2(event->eventInfoJson,"errnoVar",&eventInfo.errorValue);
    error += loadItemString(event->eventInfoJson,"errnoVar",&eventInfo.errorMsg);

    if (error > 0 )
        printFatalError("Wrong event type, eventInfoJson type is not EvenInfo_SimulationError.");
    
    return eventInfo;
}


/**
 * The function creates event information for a simulation user created event.
 * 
 * @param event A pointer to an Event struct.
 * @param info The "info" parameter is of type EvenInfo_SimulationUserCreated, which is a struct
 * containing information about a user created in a simulation. It has the following fields:
 */
void createEventInfoFor_SimulationUserCreated(Event* event, EvenInfo_SimulationUserCreated info){

    event->eventInfoJson = cJSON_CreateObject();
    cJSON_AddNumberToObject(event->eventInfoJson,"id",info.userId);
    cJSON_AddNumberToObject(event->eventInfoJson,"age",info.userAge);
    cJSON_AddNumberToObject(event->eventInfoJson,"vipPass",info.hasVipPass);
}
void general_createEventInfoFor_SimulationUserCreated(CreateEventInfo_Params param){
    createEventInfoFor_SimulationUserCreated(param.event, *((EvenInfo_SimulationUserCreated*)(param.eventInfo)) );
}

/**
 * The function `getInfoEvent_SimulationUserCreated` extracts information from a JSON object and
 * returns an `EvenInfo_SimulationUserCreated` struct.
 * 
 * @param event The "event" parameter is a pointer to an Event structure. This structure contains
 * information about the event, including the event type and the eventInfoJson field, which is a JSON
 * object containing additional information specific to the event type.
 * 
 * @return an object of type EvenInfo_SimulationUserCreated.
 */
EvenInfo_SimulationUserCreated getInfoEvent_SimulationUserCreated(Event* event){

    if(!event->eventInfoJson)
        printFatalError("Wrong event type not SimulationUserCreated, eventInfoJson is not defined.");

    EvenInfo_SimulationUserCreated eventInfo;
    int error = loadItemNumber2(event->eventInfoJson,"id",&eventInfo.userId);
    error += loadItemNumber2(event->eventInfoJson,"age",&eventInfo.userAge);
    error += loadItemNumber2(event->eventInfoJson,"vipPass",&eventInfo.hasVipPass);

    if (error > 0 )
        printFatalError("Wrong event type, eventInfoJson type is not EvenInfo_SimulationUserCreated.");
    
    return eventInfo;
}

char* extractEvent_SimulationUserCreated(Event* event){

    EvenInfo_SimulationUserCreated eventInfo = getInfoEvent_SimulationUserCreated(event);
    char* eventInfoString = "";
    asprintf(&eventInfoString,"Id:%d Age:%d Vip:%d",eventInfo.userId,eventInfo.userAge,eventInfo.hasVipPass); 
//    printf("%s",eventInfoString);
    return eventInfoString;
}

/**
 * The function asyncCreateEvent_UserCreated creates an event asynchronous
 * @param date The date parameter is of type Date and represents the date of the event being created.
 * @param eventInfo The eventInfo parameter is of type EvenInfo_SimulationUserCreated.
 * @param eventInfo_estimatedSize The parameter `eventInfo_estimatedSize` is the estimated size of the
 * `eventInfo` object in bytes.
 * @param handler The `handler`is a callback function that will be called when the event is created asynchronously.
 */
void asyncCreateEvent_UserCreated(Date date,EvenInfo_SimulationUserCreated eventInfo,int eventInfo_estimatedSize,EventMsgHandler handler){

    CreateEvent_AsyncParam* parameters = create_CreateEvent_AsyncParam(
                                            SIMULATOR_EVENT,
                                            SIMULATION_USER_CREATED,
                                            date,
                                            general_createEventInfoFor_SimulationUserCreated,
                                            eventInfo_estimatedSize,
                                            handler
                                        );

    EvenInfo_SimulationUserCreated* a = (EvenInfo_SimulationUserCreated*)malloc(sizeof(EvenInfo_SimulationUserCreated));
    *a = eventInfo;
    parameters->eventInfo = a;
    createDetachThread(asyncCreateEvent,parameters);
}

/**
 * The function creates event information for a simulation message by adding a message string to the
 * event's JSON object.
 * 
 * @param event The "event" parameter is a pointer to an Event struct. 
 * 
 * @param info The "info" parameter is of type EvenInfo_SimulationMessage.
 */
void createEventInfoFor_SimulationMessage(Event* event, EvenInfo_SimulationMessage info){

    event->eventInfoJson = cJSON_CreateObject();
    cJSON_AddStringToObject(event->eventInfoJson,"msg",info.msg);
}
void general_createEventInfoFor_SimulationMessage(CreateEventInfo_Params param){
    createEventInfoFor_SimulationMessage(param.event, *((EvenInfo_SimulationMessage*)(param.eventInfo)) );
}

/**
 * The function asyncCreateEvent_UserCreated creates an event asynchronous
 * @param date The date parameter is of type Date and represents the date of the event being created.
 * @param eventInfo The eventInfo parameter is of type EvenInfo_SimulationMessage.
 * @param eventInfo_estimatedSize The parameter `eventInfo_estimatedSize` is the estimated size of the
 * `eventInfo` object in bytes.
 * @param handler The `handler`is a callback function that will be called when the event is created asynchronously.
 */
void asyncCreateEvent_SimulationMessage(Date date, EvenInfo_SimulationMessage eventInfo,int eventInfo_estimatedSize, EventMsgHandler handler){

    CreateEvent_AsyncParam* parameters = create_CreateEvent_AsyncParam(
                                            SIMULATOR_EVENT,
                                            SIMULATION_MESSAGE,
                                            date,
                                            general_createEventInfoFor_SimulationMessage,
                                            eventInfo_estimatedSize,
                                            handler
                                        );

    EvenInfo_SimulationMessage* a = (EvenInfo_SimulationMessage*)malloc(sizeof(EvenInfo_SimulationMessage));
    *a = eventInfo;
    parameters->eventInfo = a;
    createDetachThread(asyncCreateEvent,parameters);
}
/**
 * The function `getInfoEvent_SimulationMessage` extracts information from an event object of type
 * `SimulationMessage` and returns it as an `EvenInfo_SimulationMessage` object.
 *
 * @param event The parameter "event" is a pointer to an Event object.
 *
 * @return an object of type EvenInfo_SimulationMessage.
 */
EvenInfo_SimulationMessage getInfoEvent_SimulationMessage(Event* event){

    if(!event->eventInfoJson)
        printFatalError("Wrong event type not SimulationMessage, eventInfoJson is not defined.");

    EvenInfo_SimulationMessage eventInfo;
    int error = loadItemString(event->eventInfoJson,"msg",&eventInfo.msg);

    if (error > 0 )
        printFatalError("Wrong event type, eventInfoJson type is not EvenInfo_SimulationMessage.");
    
    return eventInfo;
}

/**
 * The function creates event information for an attraction event by adding the attraction name to the
 * event's JSON object.
 * 
 * @param event A pointer to an Event struct, which contains information about an event.
 * @param info The "info" parameter is of type EventInfo_AttractionEvent, which is a struct or class
 * that contains information about an attraction event. It likely has a member variable called
 * "attractionName" which is a string representing the name of the attraction.
 */
void createEventInfoFor_AttractionEvent(Event *event, EventInfo_AttractionEvent info)
{
    event->eventInfoJson = cJSON_CreateObject();
    cJSON_AddStringToObject(event->eventInfoJson,"attractionName",info.attractionName);
}
void general_createEventInfoFor_AttractionEvent(CreateEventInfo_Params param){
    createEventInfoFor_AttractionEvent(param.event, *((EventInfo_AttractionEvent*)(param.eventInfo)) );
}

EventInfo_AttractionEvent getInfoEvent_AttractionEvent(Event* event){
    
    if (!event->eventInfoJson)
        printFatalError("Wrong event type not AttractionEvent, eventInfoJson is not defined.");
    
    EventInfo_AttractionEvent eventInfo;
    int error = loadItemString(event->eventInfoJson,"attractionName", &eventInfo.attractionName);

    if (error >0)
        printFatalError("Wrong event type, eventInfoJson type is not EventInfo_AttractionEvent.");
    
    return eventInfo;
}

void asyncCreateEvent_AttractionEvent(Date date, EventInfo_AttractionEvent eventInfo, int eventInfo_estimatedSize, EventMsgHandler handler){
    
    CreateEvent_AsyncParam* parameters = create_CreateEvent_AsyncParam(
                                            SIMULATOR_EVENT,
                                            SIMULATION_MESSAGE,
                                            date,
                                            general_createEventInfoFor_AttractionEvent,
                                            eventInfo_estimatedSize,
                                            handler
                                        );

    EventInfo_AttractionEvent* a = (EventInfo_AttractionEvent*)malloc(sizeof(EventInfo_AttractionEvent));
    *a = eventInfo;
    parameters->eventInfo = a;
    createDetachThread(asyncCreateEvent,parameters);
}

/**
 * The function "eventToString" takes an Event struct.
 * 
 * @param event The event structure that contains information about the event type, event, and date.
 * @param func The parameter "func" is a function pointer that points to a function which converts the
 * eventInfo into a string representation. This function takes in the eventInfo as a parameter and
 * returns a char* (string). Can Be NULL if Event has no eventInfoJSON
 * @param eventInfo The eventInfo parameter is a void pointer that can be used to pass additional
 * information about the event.
 * 
 * @return a string representation of an event.
 */
char* eventToString(Event event, eventInfoToString func){

    //asprintf(&strEvent,"event:%s ",getEventName(event.eventType,event.event));
    char* strEvent = "event:";
    char* eventName = getEventName(event.eventType,event.event);
    char* date = dateToString(event.date);
    char* result = "";

    if(func && event.eventInfoJson){
       // char* funcResult = func(&event);
        asprintf(&result, "%s %s %s %s.",strEvent,eventName,date, func(&event));
    }else
        asprintf(&result, "%s %s %s.",strEvent,eventName,date);

    return result;
}

/**
 * The function "getEventName" returns the name of an event based on its type and index.
 * 
 * @param type The type parameter is of type EventType, which is an enumeration representing different
 * types of events. It can have values such as SIMULATOR_EVENT, PARK_EVENT, ATTRACTION_EVENT, or
 * USER_EVENT.
 * @param event The event parameter is an integer that represents the specific event within the given
 * EventType.
 * 
 * @return a pointer to a character array (char*) which represents the name of the event.
 */
char* getEventName(EventType type , int event){

    switch(type){
        case(SIMULATOR_EVENT):{
            return simulatorEventNames[event];
        }
        case(PARK_EVENT):{
            return parkEventNames[event];
        }
        case(ATTRACTION_EVENT):{
            return attractionEventNames[event];
        }
        case(USER_EVENT):{
            return userEventNames[event];
        }
    }

}

