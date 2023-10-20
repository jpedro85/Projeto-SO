#include "file.h"

#include <stdio.h>
#include <stdlib.h>

char *readJSON(char *fileName);

/**
 * The function `loadAndParseJSONFromFile` loads and parses a JSON file, returning a cJSON object if
 * successful or NULL if there was an error.
 * 
 * @param fileName The `fileName` parameter is a string that represents the name of the file from which
 * the JSON content will be loaded and parsed.
 * 
 * @return a pointer to a cJSON structure.
 */
cJSON *loadAndParseJSONFromFile(char *fileName)
{
    char *jsonContent = readJSON(fileName);

    cJSON *loadedContent = cJSON_Parse(jsonContent);
    free(jsonContent);

    if (loadedContent == NULL)
    {
        const char *errorPtr = cJSON_GetErrorPtr();
        if (errorPtr != NULL)
        {
            printf("Loading Error: %s\n", errorPtr);
        }
        cJSON_Delete(loadedContent);
        return NULL;
    }
    return loadedContent;
}

/**
 * The function `readJSON` reads the contents of a JSON file and returns it as a character array.
 *
 * @param fileName The fileName parameter is a string that represents the name of the JSON file that
 * you want to read.
 *
 * @return The function `readJSON` returns a pointer to a character array (`char *`).
 */
char *readJSON(char *fileName)
{
    FILE *fileReader = fopen(fileName, "r");

    if (fileReader == NULL)
    {
        perror("Error: Unable to open the file.\n");
        return NULL;
    }

    // Calculates the file size
    fseek(fileReader, 0, SEEK_END);
    size_t fileSize = ftell(fileReader);
    fseek(fileReader, 0, SEEK_SET);

    if (fileSize == -1)
    {
        perror("Failed to determine file size");
        fclose(fileReader);
        return NULL;
    }

    // Allocates buffer size to same size as the file
    char *buffer = (char *)malloc(fileSize);
    if (buffer == NULL)
    {
        perror("Failed to allocate memory");
        fclose(fileReader);
        return NULL;
    }

    size_t contentRead = fread(buffer, 1, fileSize, fileReader);
    fclose(fileReader);

    if (contentRead != fileSize)
    {
        perror("Failed to read file");
        fclose(fileReader);
        free(buffer);
        return NULL;
    }

    return buffer;
}