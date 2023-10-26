#include <stdio.h>
#include <stdlib.h>
#include "registo.h"
#include "../common/events.h"

void writeRecord(FILE* file, int entity, int event, int time) {
    char* strEvent = eventMessage(event);
    char* string = NULL;

    // Calculate the length of the string
    int string_length = entity != 0 ? snprintf(NULL, 0, " %d %d %s", entity, time, strEvent) : snprintf(NULL, 0, " %d %s", time, strEvent);

    // Check if snprintf was successful
    if (string_length < 0) {
        fprintf(stderr, "Failed to determine the string length.\n");
        free(strEvent);
        return;
    }

    // Allocate memory for the string
    string = (char*)malloc(string_length + 1);

    // Check if memory allocation was successful
    if (string == NULL) {
        fprintf(stderr, "Failed to allocate memory for the string.\n");
        free(strEvent);
        return;
    }

    // Create the formatted string
    if (entity != 0) {
        snprintf(string, string_length + 1, " %d %d %s", entity, time, strEvent);
    } else {
        snprintf(string, string_length + 1, " %d %s", time, strEvent);
    }

    // Write the string to the file
    fprintf(file, "%s\n", string);

    free(string);
}
