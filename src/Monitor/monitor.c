#include "registo.h"
#include "../events.h"

int main() {
    FILE* file = fopen("registo.txt", "w");
    if (file == NULL) {
        fprintf(stderr, "Failed to open the file for writing.\n");
        return 1; // Return an error code to indicate failure
    }

    writeRecord(file, 1, 2, 3);

    fclose(file); // Close the file

    return 0; // Return 0 to indicate success
}
