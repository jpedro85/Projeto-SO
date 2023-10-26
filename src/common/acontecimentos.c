#include "acontecimentos.h"

//o funcionamento da função seria "idUtilizador hora "+mensagemAcontecimento() 
/**
 * This function returns a message based on the given event.
 * 
 * @param acontecimento The parameter "acontecimento" is an integer that represents a specific event or
 * occurrence.
 */
char* mensagemAcontecimento(int acontecimento){
    switch(acontecimento){
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