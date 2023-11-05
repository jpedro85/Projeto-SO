//#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#include "../consoleAddons.h"
#include "../linked_list.h"

/**
 * The function `sendMsg` sends a message to a file descriptor by writing the message in chunks until
 * all characters have been written.
 * 
 * @param sendTo_fd The file descriptor of the socket or file to which the message should be sent.
 * @param msg A pointer to a character array containing the message to be sent.
 * 
 * @return an integer value. If the write operation is successful, it returns 0. If there is an error,
 * it returns the value of the errno variable, which indicates the specific error that occurred.
 */
int sendMsg(int sendTo_fd, char* msg){

    int charLeft = strlen(msg);
    int charWritten = 0;

    do{

        charWritten = write(sendTo_fd, msg, charLeft);

        if(charWritten < 0)
            return errno;

        charLeft -= charWritten;
        msg += charWritten;

        
    }while(charLeft > 0);

    return 0;

}

/**
 * The function `recvMsg` reads characters from a file descriptor until it encounters a newline or null
 * character, dynamically allocating memory for the result and resizing the buffer if necessary.
 * 
 * @param recvFrom_fd The file descriptor from which to receive the message.
 * @param bufferSize The bufferSize parameter specifies the size of the buffer used to store the
 * received message. It determines the maximum number of characters that can be read at once from the
 * recvFrom_fd file descriptor.
 * @param result result is a double pointer to a char. It is used to store the received message.
 * 
 * @return an integer value. If the function is successful, it returns 0. If there is an error, it
 * returns an error code.
 */
int recvMsg(int recvFrom_fd, int bufferSize, char** result){

    char readdenChar;
    int charsReadden = 0;
    int totalCharsReadden = 0;
  //  int numberOffResizes = 0;

    *result = malloc(sizeof(char)*bufferSize);
    char* bufferPointer = *result;
    char* bufferResize;
    
    do{

        charsReadden = read(recvFrom_fd, &readdenChar, 1);
        if(charsReadden < 0)
            return errno;

        *bufferPointer = readdenChar;
        totalCharsReadden++;
        
        
        if( (totalCharsReadden % bufferSize) == 0){ // if the buffer is full

            bufferResize = realloc(*result, sizeof(char) * (totalCharsReadden + bufferSize) );
         //   numberOffResizes++;

            if(bufferResize == NULL){
                free(*result);
                return ENOMEM;
            }

            *result = bufferResize;
            bufferPointer = *result + totalCharsReadden;

        } else 
            bufferPointer++;

    }while ( readdenChar != '\n' && readdenChar != '\0' /*&& numberOffResizes <= maxBufferResizes */);   

    return 0; 
}