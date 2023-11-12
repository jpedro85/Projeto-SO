#include <string.h>
#include <stdio.h>

#include "events.h"
#include "consoleAddons.h"
#include "file.h"

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
    
    error += loadItemNumber(eventJson,"eventType",&(event.eventType));
    error += loadItemNumber(eventJson,"event",&(event.event));
    error += loadItemNumber(eventJson,"day",&(event.date.day));
    error += loadItemNumber(eventJson,"hour",&(event.date.hour));
    error += loadItemNumber(eventJson,"minute",&(event.date.minute));
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

    if(event.eventInfoJson)
        cJSON_AddItemToObject(eventJson,"eventInfo",event.eventInfoJson);

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
    int error = loadItemNumber(event->eventInfoJson,"errnoVar",&eventInfo.errorValue);
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
    int error = loadItemNumber(event->eventInfoJson,"id",&eventInfo.userId);
    error += loadItemNumber(event->eventInfoJson,"age",&eventInfo.userAge);
    error += loadItemNumber(event->eventInfoJson,"vipPass",&eventInfo.hasVipPass);

    if (error > 0 )
        printFatalError("Wrong event type, eventInfoJson type is not EvenInfo_SimulationUserCreated.");
    
    return eventInfo;
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
 * The function "eventToString" takes an Event struct.
 * 
 * @param event The event structure that contains information about the event type, event, and date.
 * @param func The parameter "func" is a function pointer that points to a function which converts the
 * eventInfo into a string representation. This function takes in the eventInfo as a parameter and
 * returns a char* (string).
 * @param eventInfo The eventInfo parameter is a void pointer that can be used to pass additional
 * information about the event.
 * 
 * @return a string representation of an event.
 */
char* eventToString(Event event, eventInfoToString func, void* eventInfo){

    //asprintf(&strEvent,"event:%s ",getEventName(event.eventType,event.event));
    char* strEvent = "event:";
    strcat(strEvent,getEventName(event.eventType,event.event));
    strcat(strEvent,dateToString(event.date));

    if(func && eventInfo)
        strcat(strEvent,func(eventInfo));

    return strEvent;
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