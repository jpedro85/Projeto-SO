#include "client.h"
#include <stdlib.h>

/**
 * The function `freeClientValues` frees the memory allocated for a `Client` object.
 * 
 * @param value The parameter `value` is a void pointer that represents the memory address of a client
 * object.
 */
void* freeClientValues(void* value){

    Client* client = (Client*)value;
    free(client);
}