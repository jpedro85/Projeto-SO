#include "globals.h"

void initParkSemaphores(){
    sem_init(&park.parkVacancy,0,park.parkCapacity);
}