#include <stdio.h>
#include "consoleAddons.h"
#include "mutexAddons.h"

/**
 * The function initializes a mutex and prints an error message if it fails to do so.
 * 
 * @param mutex The mutex parameter is a pointer to a pthread_mutex_t variable. 
 * @param name The name parameter is a string that represents the name of the mutex. 
 */
void mutex_init(pthread_mutex_t * mutex, char* name){
     if (pthread_mutex_init(mutex,NULL) < 0){
        printf("\033[1;31m Can not lock %s mutex\033[1;0m",name); 
        printFatalError("");
    }
}

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


/**
 * The function creates a detached thread that runs the specified function with the given parameters.
 * 
 * @param func The func parameter is a pointer to the function that will be executed by the new thread.
 * This function should have the following signature: void* func(void* parameters), where parameters is
 * a pointer to any data that needs to be passed to the thread function.
 * @param parameters The "parameters" parameter is a void pointer that can be used to pass any
 * additional data or arguments to the thread function.
 */
void create_DetachThread(ThreadFunction func , void* parameters , char* errorMsg){

    pthread_t thread;
    pthread_attr_t detachedThread;
    pthread_attr_init(&detachedThread);
    pthread_attr_setdetachstate(&detachedThread,PTHREAD_CREATE_DETACHED);

    if( pthread_create( &thread, &detachedThread, func , parameters) ){
        printf("\033[1;31mCan not create thread for %s\033[1;0m\n", errorMsg); 
        printFatalError("Closing");
    }

    pthread_attr_destroy(&detachedThread);
}


/**
 * The function initializes a read-write lock and prints an error message if initialization fails.
 * 
 * @param rdLock The parameter `rdLock` is a pointer to a `pthread_rwlock_t` variable. 
 * @param name The name parameter is a string that represents the name of the read-write lock. 
 */
void rwlock_init(pthread_rwlock_t * rdLock, char* name){

    if (pthread_rwlock_init(rdLock,NULL) < 0){
        printf("\033[1;31m Can not init %s rwLock\033[1;0m",name); 
        printFatalError("");
    }
}

/**
 * The function `try_rdlock` attempts to acquire a read lock on a given `pthread_rwlock_t` variable and
 * returns an error code if unsuccessful.
 * 
 * @param rdLock A pointer to a pthread_rwlock_t variable, which represents a read-write lock.
 * @param name A string representing the name of the rdLock.
 * 
 * @return the error code. If the error code is not EBUSY or 0, it means that the tryrdlock operation
 * failed, and the function returns -1. Otherwise, it returns the error code.
 */
int try_rdlock(pthread_rwlock_t * rdLock, char* name){

    int error = 0;
    error = pthread_rwlock_tryrdlock(rdLock);
    if(error != EBUSY && error != 0){
        printf("\033[1;31m Can not tryReadLock %s rwLock\033[1;0m",name); 
        printFatalError("");
        return -1;
    }else
        return error;
}

/**
 * The function `try_wrlock` attempts to acquire a write lock on a given read-write lock, and returns
 * an error code if unsuccessful.
 * 
 * @param rdLock A pointer to a pthread_rwlock_t variable, which represents a read-write lock.
 * @param name The name of the rdLock that is being tried for write locking.
 * 
 * @return the value of the `error` variable.
 */
int try_wrlock(pthread_rwlock_t * rdLock, char* name){

    int error = 0;
    error = pthread_rwlock_trywrlock(rdLock);
    if(error != EBUSY && error != 0){
        printf("\033[1;31m Can not tryWriteLock %s rwLock\033[1;0m",name); 
        printFatalError("");
        return -1;
    }else
        return error;
}


void readlock(pthread_rwlock_t * rdLock, char* name){

    if (pthread_rwlock_rdlock(rdLock) < 0){
        printf("\033[1;31m Can not rdLock %s rwLock\033[1;0m",name); 
        printFatalError("");
    }

}
void writelock(pthread_rwlock_t * rdLock, char* name){

    if (pthread_rwlock_wrlock(rdLock) < 0){
        printf("\033[1;31m Can not wrLock %s rwLock\033[1;0m",name); 
        printFatalError("");
    }
}

/**
 * The function `rwlock_unlock` unlocks a read-write lock and prints an error message if unlocking
 * fails.
 * 
 * @param rdLock The parameter `rdLock` is a pointer to a `pthread_rwlock_t` variable, which represents
 * a read-write lock.
 * @param name The name of the rdLock that is being unlocked.
 */
void rwlock_unlock(pthread_rwlock_t *rdLock, char* name){

    if (pthread_rwlock_unlock(rdLock) < 0){
        printf("\033[1;31m Can not unlock %s rdLock\033[1;0m",name); 
        printFatalError("");
    }
}


/**
 * The function `semInit` initializes a semaphore with a specified starting value and prints an error
 * message if the initialization fails.
 * 
 * @param sem A pointer to the semaphore variable that needs to be initialized.
 * @param startValue The startValue parameter is the initial value of the semaphore. It determines the
 * number of resources that are initially available for the semaphore.
 * @param name The name of the semaphore.
 */
void semInit(sem_t* sem, unsigned int startValue, char* name){

    if (sem_init(sem, 0,startValue) < 0){
        printf("\033[1;31m Can not init %s semaphore \033[1;0m",name); 
        printFatalError("");
    }
}

/**
 * The function `semWait` waits for a semaphore to become available and prints an error message if it
 * fails.
 * 
 * @param sem The parameter "sem" is a pointer to a semaphore object. It is used to identify and access
 * the semaphore that needs to be waited on.
 * @param name The name of the semaphore.
 */
void semPost(sem_t *sem, char* name){

    if (sem_post(sem) < 0){
        printf("\033[1;31m Can not post %s semaphore \033[1;0m",name); 
        printFatalError("");
    }
}

/**
 * The function `semWait` waits for a semaphore to become available and prints an error message if it
 * fails.
 * 
 * @param sem The parameter "sem" is a pointer to a semaphore object. It is used to identify and access
 * the semaphore that needs to be waited on.
 * @param name The name of the semaphore.
 */
void semWait(sem_t *sem, char* name){

    if (sem_wait(sem) < 0){
        printf("\033[1;31m Can not wait %s semaphore \033[1;0m",name); 
        printFatalError("");
    }
}

int trySemWait(sem_t *sem, char* name){
    int error = 0;
    error = sem_trywait(sem);
    if(error != EAGAIN && error != 0){
        printf("\033[1;31m Can not tryWait at %s Semaphore\033[1;0m",name); 
        printFatalError("");
        return -1;
    }else
        return error;
}