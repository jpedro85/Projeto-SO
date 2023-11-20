#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#include "attraction.h"

pthread_mutex_t attractionLock;

void initializeAttraction(Attraction *attraction) {
    attraction->isOpen = false;
    attraction->isRunning = false;

    // Initialize scheduleList, currentAttendance, and waitingLine
    initializeList(&attraction->scheduleList);
    initializeList(&attraction->currentAttendance);
    initializeList(&attraction->waitingLine);

    // Initialize attraction-specific semaphore
    sem_init(&attraction->attractionSemaphore, 0, attraction->rideCapacity);

    // Initialize attraction-specific lock
    pthread_mutex_init(&attraction->attractionLock, NULL);
}


void enterAttraction(User *client, Attraction *attraction) {
    // Use semaphore to control the number of clients in the attraction
    sem_wait(&attraction->attractionSemaphore);

    pthread_mutex_lock(&attraction->attractionLock);

    // Add client to the current attendance list
    appendNode(&attraction->currentAttendance, createNode(client));

    pthread_mutex_unlock(&attraction->attractionLock);
}

void leaveAttraction(User *client, Attraction *attraction) {
    pthread_mutex_lock(&attraction->attractionLock);

    // Remove client from the current attendance list
    removeNode(&attraction->currentAttendance, client);

    pthread_mutex_unlock(&attraction->attractionLock);

    // Release the semaphore to indicate a spot is available in the attraction
    sem_post(&attraction->attractionSemaphore);
}

void *simulateAttraction(void *attractionPtr) {
    Attraction *attraction = (Attraction *)attractionPtr;

    while (1) {
        // Check if the park is still open
        if (!attraction->isOpen) {
            pthread_exit(NULL);
        }

        // Check if the attraction is currently running
        if (attraction->isRunning) {
            pthread_mutex_lock(&attractionLock);

            // Simulate the attraction ride
            printf("Attraction %s is running!\n", attraction->name);
            usleep(attraction->duration_ms * 1000);

            // Release clients from the attraction
            Node *currentNode = attraction->currentAttendance.head;
            while (currentNode != NULL) {
                User *client = (User *)currentNode->data;
                leaveAttraction(client, attraction);
                currentNode = currentNode->next;
            }

            pthread_mutex_unlock(&attractionLock);

            // Set the attraction to not running
            attraction->isRunning = false;
        }

        // Check if there are clients in the waiting line
        if (!isListEmpty(&attraction->waitingLine)) {
            pthread_mutex_lock(&attractionLock);

            // Move clients from the waiting line to the current attendance
            int clientsToMove = attraction->rideCapacity - getListLength(&attraction->currentAttendance);
            for (int i = 0; i < clientsToMove; ++i) {
                if (isListEmpty(&attraction->waitingLine)) {
                    break; // No more clients in the waiting line
                }

                Node *node = removeFront(&attraction->waitingLine);
                User *client = (User *)node->data;
                enterAttraction(client, attraction);
                free(node);
            }

            // Set the attraction to running
            attraction->isRunning = true;

            pthread_mutex_unlock(&attractionLock);
        }

        // Wait for a short duration before checking again
        usleep(1000);
    }
}

// Other functions related to attraction operations can be added as needed
