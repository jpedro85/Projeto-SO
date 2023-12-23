//#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>

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

    int charLeft = strlen(msg) + 1;// +1 because of \0
    int charWritten = 0;

    do{
        
        charWritten = write(sendTo_fd, msg, charLeft);//does not return erro immediately when the other end closes the stream;

        if(charWritten == 0 && charLeft > 0)
            return CONNECTION_CLOSED;
        else if(charWritten < 0){
            return errno;
        }
        
        charLeft -= charWritten;
        msg += charWritten;

    }while(charLeft > 0);
    
   // charWritten = write(sendTo_fd, "\0", 1);
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
char* recvMsg(int recvFrom_fd, int bufferSize, int maxResizes){

    if(bufferSize <= 0 || maxResizes < 0){
        printError("Wrong Parameters");
        return NULL;
    }

    char readdenChar;
    int charsReadden, totalCharsReadden, numberOffResizes = 0;
    int totalBufferSize = bufferSize;

    char* strReadden = malloc(sizeof(char)*bufferSize);
    if (strReadden == NULL) {
        // Handle the error, probably by logging and exiting the function or program
        fprintf(stderr, "Failed to allocate memory for the strReadden.\n");
        return NULL; // or handle error as appropriate
    }
    char* bufferPointer = strReadden;
    char* strBufferResize;

    do{

        charsReadden = read(recvFrom_fd, &readdenChar, 1);
        if(charsReadden < 0){
            printError("Can not read from file descriptor.");
            printError(strerror(errno));
            free(strReadden);
            return NULL;
        }else if(charsReadden == 0){
            printError("Other side closed the connection");
            errno = CONNECTION_CLOSED;
            free(strReadden);
            return NULL;
        }

        *bufferPointer = readdenChar;
        totalCharsReadden++;
        
        // if the buffer is full
        if( (totalCharsReadden % bufferSize) == 0){ 

            totalBufferSize = totalCharsReadden + bufferSize;
            strBufferResize = realloc(strReadden, sizeof(char) * (totalBufferSize) );
            numberOffResizes++;

            if(strBufferResize == NULL){
                free(strReadden);
                return NULL;
            }

            if(numberOffResizes > maxResizes){
                errno = ENOBUFS;
                free(strReadden);
                return NULL;
            }

            strReadden = strBufferResize;
            bufferPointer = strReadden + totalCharsReadden;

        } else 
            bufferPointer++;

    }while ( readdenChar != '\n' && readdenChar != '\0' /*&& numberOffResizes <= maxBufferResizes */);   
    char* str = (char*)malloc(sizeof(char)*(totalCharsReadden+1) );
    if (str == NULL) {
        // Handle the error, probably by logging and exiting the function or program
        fprintf(stderr, "Failed to allocate memory for the string.\n");
        return NULL; // or handle error as appropriate
    }
    bzero(str, totalCharsReadden+1 ); // clean allocated str

    memcpy(str, strReadden, totalCharsReadden);
    str[totalCharsReadden] = '\0'; //  adding last character

    bzero(strReadden,totalBufferSize); // clean buffer
    free(strReadden); // free memory

    return str; 
}

//free SendedTo list Item.
void* freeSendedToItem(void* value){
    free( (int*) value );
}

/**
 * The function `freeMsgValues` frees the memory allocated in `Msg` struct, including its `msg`
 * string and the linked list `sendedTo`.
 * 
 * @param value The parameter "value" is a void pointer that is being casted to a pointer of type
 * "Msg".
 */
void* freeMsgValues(void* value){

    Msg* msgToFree = (Msg*)value;  

    if(msgToFree->msg){
        free( msgToFree->msg ); //freeing string
    }
  
    if(msgToFree->sendedTo.length > 0){
        clear_linkedListItemsValueWithFunc( &(msgToFree->sendedTo) , freeSendedToItem ); // clear list sendedTo
    }
}
