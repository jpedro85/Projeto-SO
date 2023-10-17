#include <stdio.h>
#include <stdlib.h>
#include "registo.h"
#include "../acontecimentos.h"

void escreveRegisto(FILE* file, int entity, int acontecimento, int tempo) {
    char* strAcontecimento = mensagemAcontecimento(acontecimento);
    char* string = NULL;

    // Calculate the length of the string
    int string_length = entity != 0 ? snprintf(NULL, 0, " %d %d %s", entity, tempo, strAcontecimento) : snprintf(NULL, 0, " %d %s", tempo, strAcontecimento);

    // Check if snprintf was successful
    if (string_length < 0) {
        fprintf(stderr, "Failed to determine the string length.\n");
        free(strAcontecimento);
        return;
    }

    // Allocate memory for the string
    string = (char*)malloc(string_length + 1);

    // Check if memory allocation was successful
    if (string == NULL) {
        fprintf(stderr, "Failed to allocate memory for the string.\n");
        free(strAcontecimento);
        return;
    }

    // Create the formatted string
    if (entity != 0) {
        snprintf(string, string_length + 1, " %d %d %s", entity, tempo, strAcontecimento);
    } else {
        snprintf(string, string_length + 1, " %d %s", tempo, strAcontecimento);
    }

    // Write the string to the file
    fprintf(file, "%s\n", string);

    free(string);
    //free(strAcontecimento);
}
