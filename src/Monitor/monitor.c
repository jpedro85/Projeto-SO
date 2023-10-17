#include "registo.h"
#include "../acontecimentos.h"

int main() {
    FILE* file = fopen("registo.txt", "w");
    if (file == NULL) {
        fprintf(stderr, "Failed to open the file for writing.\n");
        return 1; // Return an error code to indicate failure
    }

    int entity = 1; // Replace with your desired entity value
    int acontecimento = 2; // Replace with your desired acontecimento value
    int tempo = 3; // Replace with your desired tempo value

    escreveRegisto(file, entity, acontecimento, tempo);

    fclose(file); // Close the file

    return 0; // Return 0 to indicate success
}
