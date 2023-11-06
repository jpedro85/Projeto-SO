#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#include "../common/consoleAddons.h"
#include "../common/sokectUtils/socketUtil.h"
#include "../common/sokectUtils/socketComms.h"
#include "../common/linked_list.h"

#include "socketServer.h"


int serverSocket, serverLength, clientLength, pipeBrokeError;
struct sockaddr_un clientAddress, serverAddress;

LinkedList clientsList, sendMsgQueue ;

pthread_t waitingForClients_t, removeSendedMsgs_t;
int serverActive;

unsigned long int msgId;

//looks
pthread_mutex_t clientsList_mutex, serverActive_mutex, sendMsgQueue_mutex, pipeBrokeError_mutex;

/**
 * The function isServerActive checks if the server is active by acquiring and releasing a mutex lock
 * on the serverActive variable.
 * 
 * @return the value of the variable "serverActive".
 */
int isServerActive(){

    int value;
    if (pthread_mutex_lock(&serverActive_mutex) < 0) printFatalError("Can not lock serverActive_mutex.");
    value = serverActive;
    if (pthread_mutex_unlock(&serverActive_mutex) < 0) printFatalError("Can not unlock serverActive_mutex.");

    return value;
}

void* removeSendedMsgs(){

    ListItem* item;
    int index = 0;

    while(isServerActive()){
        
        if (pthread_mutex_lock(&clientsList_mutex) < 0) printFatalError("Can not lock clientsList_mutex.");
        if (pthread_mutex_lock(&sendMsgQueue_mutex) < 0) printFatalError("Can not lock sendMsgQueue_mutex.");

        ForEach_LinkedList((&sendMsgQueue), item){

            if( ((Msg*)(item->value))->numberOfSends == clientsList.length ){
            
                if (item != NULL && item->value != NULL && ((Msg*)(item->value))->msg != NULL) {

                    free( ((Msg*)(item->value))->msg); //freeing string
                
                } else {
                    printError("Can not free string sended.");
                }

                removeItemByIndex_LinkedList(&sendMsgQueue,index);

            }

            index++;
        }
        

        if (pthread_mutex_unlock(&sendMsgQueue_mutex) < 0) printFatalError("Can not unlock sendMsgQueue_mutex.");
        if (pthread_mutex_unlock(&clientsList_mutex) < 0) printFatalError("Can not unlock clientsList_mutex."); 

        index = 0;

        pthread_testcancel();
    }

}

 /**
  * The function "chooseMsgToSend" selects a message from the sendMsgQueue to be sent, based on certain
  * conditions.
  * @param messageId message to jump.
  * @return Msg* message NULL if the is no message
  */
 Msg* chooseMsgToSend(int messageId){

    Msg* message = NULL;
    ListItem* item;
    // getting msg to send
    if (pthread_mutex_lock(&sendMsgQueue_mutex) < 0) printFatalError("Can not lock sendMsgQueue_mutex.");
        
    ForEach_LinkedList((&sendMsgQueue), item){

        if( ((Msg*)(item->value))->id != messageId){
            message = (Msg*)(item->value);
        }
        
    }

    if (pthread_mutex_unlock(&sendMsgQueue_mutex) < 0) printFatalError("Can not unlock sendMsgQueue_mutex.");
    return message;

 }

/**
 * The function `sendMsgToClient` sends messages to a client using a socket connection.
 * 
 * @param client The parameter "client" is a void pointer (to int) that represents a client object. It is used
 * to identify the client for which the messages need to be sent.
 */
void* sendMsgToClient( void* client ){
    
    int socketFd = ((Client*)client)->socket ;
    int clientIndex = ((Client*)client)->index ; 
    int error;
    Msg* message;
    ListItem* item;
    int lastMessage_id;
    int retryNumber = 0;

    printf("\033[1;33mserver: client %d socket %d waiting for msg's to send.\033[1;0m\n", clientIndex , socketFd);

    while(isServerActive()){ 

        message = chooseMsgToSend(lastMessage_id);        
        if(message == NULL){
            continue; // all messages sended to client
        }

        //sending msg
        error = sendMsg( socketFd, message->msg ); //SO emits SIGPIPE if socketFd was closed by the client.

        // check send error SIGPIPE when client closes connection
        if (pthread_mutex_lock(&pipeBrokeError_mutex) < 0) printFatalError("Can not lock pipeBrokeError_mutex.");
        if(pipeBrokeError == clientIndex)
            error = CLOSED;
        if (pthread_mutex_unlock(&pipeBrokeError_mutex) < 0) printFatalError("Can not lock pipeBrokeError_mutex.");

        if(error == CLOSED){
            printf("\033[1;31mserver: could not send '%s' to client %d. Client closed connection to socket %d!\033[1;0m\n",message->msg , clientIndex, socketFd );
            break;

        } else if(error == CONNECTION_CLOSED){
                
            printf("\033[1;31mserver: could not send '%s' to client %d.Client closed connection to socket %d!\033[1;0m\n",message->msg , clientIndex, socketFd );

            if (pthread_mutex_lock(&clientsList_mutex) < 0) printFatalError("Can not lock clientsList_mutex.");
            
            if ( clientsList.length - 1 == 0){
                printWarning("No clients connected. Simulator exiting.");
                exit(1);
            }

            removeItemByIndex_LinkedList(&clientsList,clientIndex);

            if (pthread_mutex_unlock(&clientsList_mutex) < 0) printFatalError("Can not lock clientsList_mutex.");

            break;
                
        } else if(error < 0 ){

            printError( strerror(error) );
            printf("\033[1;31mserver: could not send '%s' to client %d socket %d!\033[1;0m\n",message->msg , clientIndex, socketFd );

            if( (++retryNumber) == 3 ){
                retryNumber = 0;
                lastMessage_id = message->id;
            }

            //Msg retry send next's 3 cycles . if can't jumps this message;
        } else {  

            if (pthread_mutex_lock(&sendMsgQueue_mutex) < 0) printFatalError("Can not lock sendMsgQueue_mutex.");

            message->numberOfSends++;
            lastMessage_id = message->id;

            printf("\033[1;32mserver: msg '%s' sended to client %d socket %d !\033[1;0m\n",message->msg, clientIndex, socketFd);

            if (pthread_mutex_unlock(&sendMsgQueue_mutex) < 0) printFatalError("Can not unlock sendMsgQueue_mutex.");
        }

        pthread_testcancel();
    }


}

void* checkConnection( void* client ){

    int socketFd = ((Client*)client)->socket ;
    int clientIndex = ((Client*)client)->index ; 

    while(isServerActive()){
        
        recvMsg(socketFd,1);
        if( errno == CONNECTION_CLOSED){

            printf("\033[1;31mserver: Client %d closed connection to socket %d!\033[1;0m\n", clientIndex, socketFd );

            if (pthread_mutex_lock(&pipeBrokeError_mutex) < 0) printFatalError("Can not lock pipeBrokeError_mutex.");
            if (pthread_mutex_lock(&clientsList_mutex) < 0) printFatalError("Can not lock clientsList_mutex.");

            if ( clientsList.length - 1 == 0){
                printWarning("No clients connected. Simulator exiting.");
                exit(1);
            }
            removeItemByIndex_LinkedList(&clientsList,clientIndex);

            pipeBrokeError = clientIndex;

            if (pthread_mutex_unlock(&clientsList_mutex) < 0) printFatalError("Can not lock clientsList_mutex.");
            if (pthread_mutex_unlock(&pipeBrokeError_mutex) < 0) printFatalError("Can not lock pipeBrokeError_mutex.");

            break;
            
        }else if(errno)
            printError(strerror(errno));
            
        pthread_testcancel();
    }
}

/**
 * The acceptClient function accepts a new client connection and adds it to a linked list of clients,
 * while ensuring thread safety.
 */
void acceptClient(){
    
    int newSocketClient;
    clientLength = sizeof(clientAddress);
    int clientIndex;

    newSocketClient = accept(serverSocket, (struct sockaddr *) &clientAddress, &clientLength);
    if (newSocketClient < 0){
        
        if(errno == EAGAIN || errno == EWOULDBLOCK )//try again;
            return;

        printError(strerror(errno));
        printFatalError("server:Can not accept Client.");

    } else {

        Client* client = (Client*)malloc(sizeof(Client));
        if(client == NULL)
            printFatalError("server: can not create client.");

        client->socket = newSocketClient;

        // to ensure that a msg is not send while a new client is added to the list or a client is not added will a msg is being sended.
        if( pthread_mutex_lock(&clientsList_mutex) < 0) printFatalError("Can not lock clientsList_mutex.");

        addValue_LinkedList(&clientsList,client);
        client->index = clientsList.length - 1;

        if (pthread_mutex_unlock(&clientsList_mutex) < 0) printFatalError("Can not unlock clientsList_mutex.");

        // creating a tread for handling the sending of msg to the new client.
        if ( pthread_create(&(client->thread_send), NULL, sendMsgToClient, (void*)(client) ) < 0 ){
            printFatalError("Can not create thread for sendMsgToClient.");
        }

        // creating a tread for handling the receive from client.
        if ( pthread_create(&(client->thread_recv), NULL, checkConnection, (void*)(client) ) < 0 ){
            printFatalError("Can not create thread for checkConnection.");
        }

        printf("\033[1;33mserver: client connected. Now %d are connected!\033[1;0m\n",clientsList.length);
            
    }
}

/**
 * The function waits for clients to connect and accepts them.
 */
void* waitForClients(){

    printWarning("server: waiting for Clients.");
    int active = 0;

    do{

        acceptClient();
        pthread_testcancel();

    }while(isServerActive());
    
}

/**
 * The function "waitFirstConnection" waits until there is at least one client connected.
 * 
 */
void waitFirstConnection(){

    int length = 0;
    while(1){

        if( pthread_mutex_lock(&clientsList_mutex) < 0) printFatalError("Can not lock clientsList_mutex.");
        length = clientsList.length;
        if( pthread_mutex_unlock(&clientsList_mutex) < 0) printFatalError("Can not unlock clientsList_mutex.");

        if(length > 0)
            return;   

    }
}


void sigpipe_handler(int signum) {
    printWarning("server: Tried to send for a closed connection.");
}

/**
 * The startServer function creates a socket stream, binds it to a specific address, and prepares the
 * server for receiving client connections.
 */
void startServer(){

    /* Creating Socket Stream */
    serverSocket = socket(AF_UNIX,SOCK_STREAM,0);
	if (serverSocket  < 0){
        printError(strerror(errno));
		printFatalError("server: can't open stream socket.");
    }else
        printWarning("server: socket created.");

    /* Creating address*/
	serverAddress.sun_family = AF_UNIX;
	strcpy(serverAddress.sun_path, UNIXSTREAM_PATH);
    serverLength = sizeof(serverAddress);

    /* freeing the name from the filesystem */
    if( unlink(UNIXSTREAM_PATH) < 0 && errno != ENOENT){
        printError(strerror(errno));
		printFatalError("server: can't unlink socket name.");
    }

    /* Binding the name to the socket*/
	if ( bind(serverSocket, (struct sockaddr *)&serverAddress, serverLength) < 0 ){
        printError(strerror(errno));
		printFatalError("server: can't bind stream socket.");
    }else
        printWarning("server: socket binded.");

    /* Server ready for receiving client connections */
	if( listen(serverSocket, 5) < 0){
        printError(strerror(errno));
		printFatalError("server: can't bind stream socket.");
    }else
        printWarning("server: prepared for connections.");

    //initializing the client list and send msgQueue
    initialize_LinkedList(&clientsList);
    initialize_LinkedList(&sendMsgQueue);

    serverActive = 1;
    pipeBrokeError = -1;

     signal(SIGPIPE, sigpipe_handler);
    // creating a tread for handling next connections;
    if ( pthread_create(&waitingForClients_t, NULL, waitForClients, NULL) < 0 ){
        printFatalError("Can not create thread for waitForClients.");
    }

    // creating a tread for handling the cleaning of sendMsgQueue.
    if ( pthread_create(&removeSendedMsgs_t, NULL, removeSendedMsgs, NULL) < 0 ){
        printFatalError("Can not create thread for removeSendedMsgs.");
    }

    
}

/**
 * The stopServer function closes the server socket and all client sockets in the clientsList.
 */
void stopServer(){

    if (pthread_mutex_lock(&serverActive_mutex) < 0) printFatalError("Can not lock serverActive_mutex.");
    serverActive = 0;
    if (pthread_mutex_unlock(&serverActive_mutex) < 0) printFatalError("Can not unlock serverActive_mutex.");

    close(serverSocket);

    ListItem* item;
    ForEach_LinkedList((&clientsList),item){

        if( close( ((Client*)item->value)->socket ) < 0 ){
            printf("\033[1;31merror: %s\nCould not close socket %d.\033[1;0m\n",strerror(errno),((Client*)item->value)->socket);
        }else{
            printf("\033[1;32mClosed socket %d.\033[1;0m\n",((Client*)item->value)->socket);
        }
        pthread_cancel(((Client*)item->value)->thread_send);
        pthread_cancel(((Client*)item->value)->thread_recv);
    }

    pthread_cancel(waitingForClients_t);
    pthread_cancel(removeSendedMsgs_t);

    //clean lists

    printWarning("server: server closed.");
}

void addMsgToQueue(char* msg){

    Msg* message = malloc( sizeof(Msg) );

    if( message == NULL )
        printFatalError("Can not allocate memory for message.");


    message->msg = NULL; 
    message->msg = (char*)malloc( sizeof(char) * (strlen(msg)+1) ); 
    if (message->msg == NULL) 
        printFatalError("Failed to allocate memory for message content.");

    strcpy(message->msg,msg);
    message->numberOfSends = 0;
    message->id = ++msgId;

    if (pthread_mutex_lock(&sendMsgQueue_mutex) < 0) printFatalError("Can not lock serverActive_mutex.");//presa
    addValue_LinkedList(&sendMsgQueue, message); 
    if (pthread_mutex_unlock(&sendMsgQueue_mutex) < 0) printFatalError("Can not unlock serverActive_mutex.");

}

