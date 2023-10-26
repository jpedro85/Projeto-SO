#include <stdio.h>

#include "registo.h"
#include "../common/events.h"
#include "../common/consoleAddons.h"

int main(int argc , char *argv[] ){

    printWarning("Monitor Started !");

    FILE* file = fopen("registo.txt", "w");
    if (file == NULL) {
        fprintf(stderr, "Failed to open the file for writing.\n");
        return 1; // Return an error code to indicate failure
    }

    writeRecord(file, 0, SimulationStart, 1);
    writeRecord(file, 1, UserJoined, 2);
    writeRecord(file, 2, UserJoined, 3);
    writeRecord(file, 2, UserLeft, 4);
    writeRecord(file, 1, UserLeft, 5);
    writeRecord(file, 0, SimulationStart, 6);

    fclose(file); // Close the file

    printWarning("Monitor terminated successfully");
    return 0;
}