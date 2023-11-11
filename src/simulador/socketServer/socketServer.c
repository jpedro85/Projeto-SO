#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#include "../../common/consoleAddons.h"
#include "../../common/socketUtils/socketUtil.h"
#include "../../common/socketUtils/socketComms.h"
#include "../../common/linked_list.h"
#include "../../common/cjson/cJSON.h"
#include "../../common/events.h"
#include "../../common/date.h"
#include "../user.h"
#include "../attraction.h"
#include "../park.h"

#include "socketServer.h"
#include "client.h"
#include "../../common/mutexAddons.h"

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
    lockMutex(&serverActive_mutex,"serverActive_mutex");
    value = serverActive;
    unlockMutex(&serverActive_mutex,"serverActive_mutex");

    return value;
}

void* removeSendedMsgs(){

    ListItem* item_msg;
    Msg* msgToFree = NULL;
    int index = -1;
    int remove = 0;

    while(isServerActive()){
        
        lockMutex(&clientsList_mutex,"clientsList_mutex");
        lockMutex(&sendMsgQueue_mutex,"sendMsgQueue_mutex");

        ForEach_LinkedList((&sendMsgQueue), item_msg){

            index++;

            if( ((Msg*)(item_msg->value))->sendedTo.length == clientsList.length ){
                msgToFree = (Msg*)(item_msg->value);
                remove = 1;
                break;
            }
        }

        if(index != -1 && remove){
            if(msgToFree){
                freeMsgValues(msgToFree);
                removeItemByIndex_LinkedList(&sendMsgQueue,index);
                msgToFree = NULL;
                remove = 0;
            }else //Already Sended;
                printError("Can not free msg sended.");
        }
            
        unlockMutex(&sendMsgQueue_mutex,"sendMsgQueue_mutex");
        unlockMutex(&clientsList_mutex,"clientsList_mutex");

        index = -1;

        pthread_testcancel();
    }

}

 /**
  * The function "chooseMsgToSend" selects a message from the sendMsgQueue to be sent, based on certain
  * conditions.
  * @param messageId message to jump.
  * @return Msg* message NULL if the is no message
  */
 Msg* chooseMsgToSend(int clientIndex){

    Msg* message = NULL;
    ListItem* item;
    LinkedList* sendedToList;
    ListItem* sendedToItem;
    int sendedToHasClient = 0;
    // getting msg to send
    lockMutex(&sendMsgQueue_mutex,"sendMsgQueue_mutex");

    // for each msg in queue 
    ForEach_LinkedList((&sendMsgQueue), item){

        sendedToList = &(((Msg*)(item->value))->sendedTo);
        ForEach_LinkedList( sendedToList, sendedToItem){ 

            if ( *((int*)(sendedToItem->value)) ==  clientIndex){
                sendedToHasClient = 1;
            }
        }

        if(!sendedToHasClient){ // if client is NOT in return sendedTo
            message = (Msg*)(item->value);
        }else
            sendedToHasClient = 0; // if client is in msg->SendedTo next msg
        
    }

    unlockMutex(&sendMsgQueue_mutex,"sendMsgQueue_mutex");
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
    int retryNumber = 0;

    printf("\033[1;33mserver: client %d socket %d waiting for msg's to send.\033[1;0m\n", clientIndex , socketFd);

    while(isServerActive()){ 

        message = chooseMsgToSend(clientIndex);        
        if(message == NULL)
            continue; // all messages sended to client
        
        //sending msg
        error = sendMsg( socketFd, message->msg ); //SO emits SIGPIPE if socketFd was closed by the client.

        // check send error SIGPIPE when client closes connection
        lockMutex(&pipeBrokeError_mutex,"pipeBrokeError_mutex");
        if(pipeBrokeError == clientIndex)
            error = CLOSED;
        unlockMutex(&pipeBrokeError_mutex,"pipeBrokeError_mutex");

        if(error == CONNECTION_CLOSED || error == CLOSED){   
            printf("\033[1;31mserver: could not send to client %d.Client closed connection to socket %d!\033[1;0m\n", clientIndex, socketFd );
            break;
                
        } else if(error < 0 ){

            printError( strerror(error) );
            printf("\033[1;31mserver: could not send '%s' to client %d socket %d!\033[1;0m\n",message->msg , clientIndex, socketFd );

            if( (++retryNumber) == 3 ){
                retryNumber = 0;
            }

            //Msg retry send next's 3 cycles . if can't jumps this message;
        } else {  

            lockMutex(&sendMsgQueue_mutex,"sendMsgQueue_mutex");

            addInt_LinkedList( &(message->sendedTo), clientIndex );

            printf("\033[1;32mserver: msg '%s' sended to client %d socket %d !\033[1;0m\n",message->msg, clientIndex, socketFd);

            unlockMutex(&sendMsgQueue_mutex,"sendMsgQueue_mutex");
        }

        pthread_testcancel();
    }


}

/**
 * The function `checkConnection` checks if a client has closed its connection to the server and
 * performs necessary actions.
 * 
 * @param client The parameter `client` is a void pointer that is cast to a `Client*` type inside the
 * function. It is assumed that `Client` is a struct information about the
 * client, such as the socket file descriptor (`socketFd`) and the client index (`clientIndex`).
 */
void* checkConnection( void* client ){

    int socketFd = ((Client*)client)->socket ;
    int clientIndex = ((Client*)client)->index ; 
    int numberOfClients = 0;

    while(isServerActive()){
        
        recvMsg(socketFd,1,0);
        if( errno == CONNECTION_CLOSED){

            printf("\033[1;31mserver: Client %d closed connection to socket %d!\033[1;0m\n", clientIndex, socketFd );

            lockMutex(&clientsList_mutex,"clientsList_mutex");
            numberOfClients = clientsList.length;
            removeItemByIndex_LinkedList(&clientsList,clientIndex);
            unlockMutex(&clientsList_mutex,"clientsList_mutex");
           
            lockMutex(&pipeBrokeError_mutex,"pipeBrokeError_mutex");
            pipeBrokeError = clientIndex;
            unlockMutex(&pipeBrokeError_mutex,"pipeBrokeError_mutex");

            if ( numberOfClients == 0){
                printWarning("No clients connected. Simulator exiting. ");
                exit(1);
            }

            break;
            
        }else if(errno)
            printError(strerror(errno));
            
        pthread_testcancel();
    }
}

/**
 * The function creates a new client and adds it to a linked list of clients.
 * 
 * @param clientSocket The clientSocket parameter is an integer that represents the socket file
 * descriptor for the client connection.
 * 
 * @return a pointer to a Client structure.
 */
Client* createClient( int clientSocket ){

    Client* client = (Client*)malloc(sizeof(Client));
    if(client == NULL)
        printFatalError("server: can not create client.");

    client->socket = clientSocket;

    // to ensure that a msg is not send while a new client is added to the list or prevent a client is not added while a msg is being sended.
    lockMutex(&clientsList_mutex,"clientsList_mutex");
    addValue_LinkedList(&clientsList,client);
    client->index = clientsList.length - 1;
    unlockMutex(&clientsList_mutex,"clientsList_mutex");

    return client;
}

/**
 * The function creates two threads, one for sending messages to a client and one for receiving
 * messages from a client.
 */
void createClientConnectionsHandler(Client* client){
    // creating a tread for handling the sending of msg to the new client.
    if ( pthread_create(&(client->thread_send), NULL, sendMsgToClient, (void*)(client) ) < 0 ){
        printFatalError("Can not create thread for sendMsgToClient.");
    }

    // creating a tread for handling the receive from client.
    if ( pthread_create(&(client->thread_recv), NULL, checkConnection, (void*)(client) ) < 0 ){
        printFatalError("Can not create thread for checkConnection.");
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
        printFatalError("server: can not accept Client.");

    } else {

        Client* client = createClient(newSocketClient);
        createClientConnectionsHandler(client);
    
        lockMutex(&clientsList_mutex,"clientsList_mutex");
        printf("\033[1;33mserver: client connected. Now %d are connected!\033[1;0m\n",clientsList.length);
        unlockMutex(&clientsList_mutex,"clientsList_mutex");
            
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
        
        lockMutex(&clientsList_mutex,"clientsList_mutex");
        length = clientsList.length;
        unlockMutex(&clientsList_mutex,"clientsList_mutex");

        if(length > 0)
            return;   

    }
}


void sigpipeHandler(int signum) {
    printWarning("server: Tried to send msg for a closed connection.");
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

    signal(SIGPIPE, sigpipeHandler);
    // creating a tread for handling next connections;
    if ( pthread_create(&waitingForClients_t, NULL, waitForClients, NULL) < 0 ){
        printFatalError("Can not create thread for waitForClients.");
    }

    //creating a tread for handling the cleaning of sendMsgQueue.
    if ( pthread_create(&removeSendedMsgs_t, NULL, removeSendedMsgs, NULL) < 0 ){
        printFatalError("Can not create thread for removeSendedMsgs.");
    }

    
}

/**
 * The stopServer function closes the server socket and all client sockets in the clientsList.
 */
void stopServer(){

    lockMutex(&serverActive_mutex,"serverActive_mutex");
    serverActive = 0;
    unlockMutex(&serverActive_mutex,"serverActive_mutex");

    pthread_cancel(waitingForClients_t);
    //pthread_cancel(removeSendedMsgs_t);

    lockMutex(&clientsList_mutex,"clientsList_mutex");
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
    unlockMutex(&clientsList_mutex,"clientsList_mutex");

    if (close(serverSocket) < 0){
        printError(strerror(errno));
    }

    //segmentation
    lockMutex(&sendMsgQueue_mutex,"sendMsgQueue_mutex");
    if(sendMsgQueue.length > 0)
        clear_linkedListItemsValueWithFunc(&sendMsgQueue,freeMsgValues);
    unlockMutex(&sendMsgQueue_mutex,"sendMsgQueue_mutex");

    lockMutex(&clientsList_mutex,"clientsList_mutex");
    if(clientsList.length > 0)
        clear_linkedListItemsValueWithFunc(&clientsList,freeClientValues);
    unlockMutex(&clientsList_mutex,"clientsList_mutex");

    printWarning("server: server closed.");
}

/**
 * The function `addMsgToQueue` adds a message to a queue, allocating memory for the message and its
 * content.
 * 
 * @param msg A pointer to a character array containing the message to be added to the queue.
 */
void addMsgToQueue(char* msg){

    Msg* message = malloc( sizeof(Msg) );

    if( message == NULL )
        printFatalError("Can not allocate memory for message.");

    message->msg = NULL; 
    message->msg = (char*)malloc( sizeof(char) * (strlen(msg)+1) ); 
    if (message->msg == NULL) 
        printFatalError("Failed to allocate memory for message content.");

    strcpy(message->msg,msg);
    initialize_LinkedList(&(message->sendedTo));
    message->id = ++msgId;

    lockMutex(&sendMsgQueue_mutex,"sendMsgQueue_mutex");
    addValue_LinkedList(&sendMsgQueue, message); 
    unlockMutex(&sendMsgQueue_mutex,"sendMsgQueue_mutex");

}