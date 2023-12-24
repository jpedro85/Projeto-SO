#ifndef SIMULATION_H
#define SIMULATION_H

typedef struct
{
    int averageClientArriveTime_ms;
    int toleranceClientArriveTime_ms;
    int dayLength_s;
    int dayLength_ms;
    int numberOfDaysToSimulate;
    int userMaxAge;
    int userMinAge;
    int userMinWaitingTime_ms;
    int userLeaveChance_percentage;
    int userHasVipPassChance_percentage;

}SimulationConf;



#endif