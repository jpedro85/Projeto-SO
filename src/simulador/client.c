#include "client.h"
#include <stdlib.h>

void* freeClientValues(void* value){

    Client* client = (Client*)value;
    free(client);
}