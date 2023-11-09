#include <pthread.h>
#include <stdio.h>
#include "consoleAddons.h"

/**
 * The function "lockMutex" locks a given mutex and prints an error message if the lock operation
 * fails.
 * 
 * @param mutex A pointer to the mutex variable that you want to lock.
 * @param name The name of the mutex that is being locked.
 */
void lockMutex(pthread_mutex_t* mutex , char* name){

    if (pthread_mutex_lock(mutex) < 0){
        printf("\033[1;31m Can not lock %s mutex\033[1;0m",name); 
        printFatalError("");
    }
}

/**
 * The function unlocks a mutex and prints an error message if unlocking fails.
 * 
 * @param mutex A pointer to the mutex variable that you want to unlock.
 * @param name The name of the mutex that is being unlocked.
 */
void unlockMutex(pthread_mutex_t* mutex , char* name){

    if (pthread_mutex_unlock(mutex) < 0){
        printf("\033[1;31m Can not unlock %s mutex\033[1;0m",name); 
        printFatalError("");
    }
}
