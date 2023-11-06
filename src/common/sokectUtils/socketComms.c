//#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#include "../consoleAddons.h"
#include "../linked_list.h"
#include "socketComms.h"

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

    charWritten = write(sendTo_fd, "\n", 1);
    if(charWritten < 0)
            return errno;

    return 0;

}

/**
 * The function `recvMsg` reads characters from a file descriptor until it encounters a newline or null
 * character, dynamically resizing the buffer if necessary.
 * 
 * @param recvFrom_fd The file descriptor from which to receive the message.
 * @param bufferSize The bufferSize parameter is an integer that specifies the size of the buffer used
 * to read characters from the file descriptor recvFrom_fd.
 * 
 * @return a pointer to a character array (string) that contains the received message. return null if an error ocurred
 */
char* recvMsg(int recvFrom_fd, int bufferSize){

    char* strReadden;
    char readdenChar;
    int charsReadden = 0;
    int totalCharsReadden = 0;
  //  int numberOffResizes = 0;

    strReadden = malloc(sizeof(char)*bufferSize);
    char* bufferPointer = strReadden;
    char* strBufferResize;
    
    do{

        charsReadden = read(recvFrom_fd, &readdenChar, 1);
        if(charsReadden < 0){
            printError("Can not read from file descriptor.");
            printError(strerror(errno));
            return NULL;
        }else if(charsReadden == 0){
            printError("client: Server closed connection");
            errno = CONNECTION_CLOSED;
            return NULL;
        }

        *bufferPointer = readdenChar;
        totalCharsReadden++;
        
        
        if( (totalCharsReadden % bufferSize) == 0){ // if the buffer is full

            strBufferResize = realloc(strReadden, sizeof(char) * (totalCharsReadden + bufferSize) );
         //   numberOffResizes++;

            if(strBufferResize == NULL){
                free(strReadden);
                return NULL;
            }

            strReadden = strBufferResize;
            bufferPointer = strReadden + totalCharsReadden;

        } else 
            bufferPointer++;

    }while ( readdenChar != '\n' && readdenChar != '\0' /*&& numberOffResizes <= maxBufferResizes */);   

    char* str = (char*)malloc(sizeof(char)*(totalCharsReadden+1) );
    memcpy(str, strReadden, totalCharsReadden);
    free(strReadden);

    return str; 
}