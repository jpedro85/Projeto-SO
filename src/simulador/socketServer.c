#include <errno.h>
#include <string.h>
#include <pthread.h>

#include "../common/consoleAddons.h"
#include "../common/sokectUtils/socketUtil.h"
#include "../common/linked_list.h"

int serverSocket, serverLength, clientLength;
struct sockaddr_un clientAddress, serverAddress;
LinkedList clientsList;
pthread_t waitingForClients_t;

//looks
pthread_mutex_t clientsList_mutex;

/**
 * The acceptClient function accepts a new client connection and adds it to a linked list of clients,
 * while ensuring thread safety.
 * 
 */
void acceptClient(){
    
    int newSocketClient;
    clientLength = sizeof(clientAddress);

    newSocketClient = accept(sockfd, (struct sockaddr *) &clientAddress, &clientLength)
    if (newSocketClient < 0){
        
        if(errno == EAGAIN || errno == EWOULDBLOCK )//try again;
            return;

        printError(strerror(errno));
        printFatalError("server:Can not accept Client.")

    } else {

        // to ensure that a msg is not send while a new client is added to the list or a client is not added will a msg is being sended.
        if( pthread_mutex_look(&clientsList_mutex) == 0){
            
            addInt_LinkedList(clientsList,newSocketClient);

            if (pthread_mutex_unlock(&clientsList_mutex) != 0){

                printFatalError("Can not unlock clientsList_mutex.")
            }

            printWarning("server: new client connected.");
            
        }else
            printFatalError("Can not lock clientsList_mutex.")
            
    }
}

/**
 * The function waits for clients to connect and accepts them.
 */
void waitForClients(){

    printWarning("Waiting for Clients.")
    while (1){
        acceptClient();
    }
    
}

/**
 * Creates a socket for the server and starts the server. Waits for the firs connection. 
 * Before continue creates a thread for handling possible next connections.
 */
int startServer(){

    /* Creating Socket Stream */
    serverSocket = socket(AF_UNIX,SOCK_STREAM,0);
	if (serverSocket  < 0){
        printError(strerror(errno));
		printFatalError("server: can't open stream socket");
    }else
        printWarning("server: socket created.");

    /* Creating address*/
	serverAddress.sun_family = AF_UNIX;
	strcpy(serverAddress.sun_path, UNIXSTREAM_PATH);
    serverLength = sizeof(serverAddress);

    /* freeing the name from the filesystem */
    if( unlink(UNIXSTREAM_PATH) < 0 ){
        printError(strerror(errno));
		printFatalError("server: can't bind stream socket");
    }

    /* Binding the name to the socket*/
	if ( bind(serverSocket, /*(struct sockaddr *)*/ &serverAddress , serverLength) < 0 ){
        printError(strerror(errno));
		printFatalError("server: can't bind stream socket");
    }else
        printWarning("server: socket binded.");

    /* Server ready for receiving client connections */
	if( listen(serverSocket, 5) < 0){
        printError(strerror(errno));
		printFatalError("server: can't bind stream socket");
    }else
        printWarning("server: prepared for connections.");

    // wait for firs connection, to ensure at list one is available before starting simulation;
    acceptClient();

    // creating a tread for handling next connections;
    if ( pthread_create(&waitingForClients_t, NULL, waitForClients, NULL) < 0 ){
        printFatalError("Can not create thread for waitForClients.");
    }

}

/**
 * The stopServer function closes the server socket and all client sockets in the clientsList.
 */
void stopServer(){

    close(serverSocket);

    ListItem* socket;
    ForEach_LinkedList(clientsList,socket){
        close((int)socket->value);
    }

}

