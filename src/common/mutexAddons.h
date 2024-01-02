#ifndef MUTEX_ADDONS_H
#define MUTEX_ADDONS_H

#include <pthread.h>
#include <semaphore.h>
#include <errno.h>

//todo : init exitent mutext
void mutex_init(pthread_mutex_t * mutex, char* name);
void lockMutex(pthread_mutex_t* mutex , char* name);
void unlockMutex(pthread_mutex_t* mutex , char* name);

void rwlock_init(pthread_rwlock_t * rdLock, char* name);
int try_rdlock(pthread_rwlock_t * rdLock, char* name);
int try_wrlock(pthread_rwlock_t * rdLock, char* name);
void readlock(pthread_rwlock_t * rdLock, char* name);
void writelock(pthread_rwlock_t * rdLock, char* name);
void rwlock_unlock(pthread_rwlock_t *rdLock, char* name);

typedef void* (*ThreadFunction) (void*);
void create_DetachThread(ThreadFunction func , void* parameters ,char* errorMsg);


void semInit(sem_t* sem, unsigned int startValue, char* name);
void semPost(sem_t *sem, char* name);
void semWait(sem_t *sem, char* name);
//int trySemWait(sem_t *sem, char* name);

#endif