#include "events.h"

char* eventMessage(int event){
    switch(event){
        case(SimulationStart):{
            return "/*/*/*/*/Simulation Started/*/*/*/*/";
        }
        case(SimulationTerminated):{
            return "/*/*/*/*/Simulation Terminated/*/*/*/*/";
        }
        case(UserJoined):{
            return "Joined the park";
        }
        case(UserLeft):{
            return "Left the park";
        }
    }
}