#ifndef SIMULATION_H
#define SIMULATION_H

typedef struct SimulationConf
{
    int averageClientArriveTime_ms;
    int toleranceClientArriveTime_ms;
    int dayLength_s;
    int userMaxAge;
    int userMinAge;
    int userMinWaitingTime_ms;

}SimulationConf;



#endif