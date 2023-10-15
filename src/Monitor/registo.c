#include <stdio.h>
#include <stdlib.h>
#include "registo.h"

void escreveRegisto(FILE* file, int entity, int acontecimento, int tempo) {
    char* strAcontecimento = mensagemAcontecimento(acontecimento);
    char* string;
    
    // If the id is not 0, it's a user's event; include it in the string
    if (entity != 0) {
        sprintf(&string, " %d %d %s", entity, tempo, strAcontecimento);
    } else {
        sprintf(&string, " %d %s", tempo, strAcontecimento);
    }

    // Check if asprintf was successful in allocating memory
    if (string == NULL) {
        // Handle the error, e.g., by returning or printing an error message
        fprintf(stderr, "Failed to allocate memory for the string.\n");
        return;
    }

    // Write the string to the file
    fprintf(file, "%s\n", string);
}
