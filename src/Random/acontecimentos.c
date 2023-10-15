#include "acontecimentos.h"

//o funcionamento da função seria "idUtilizador hora "+mensagemAcontecimento() 
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