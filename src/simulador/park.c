#include "globals.h"

void initParkSemaphores(){
    sem_init(&parkVacancy,0,park.parkCapacity);
}