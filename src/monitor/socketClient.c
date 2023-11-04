#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

#include "../common/consoleAddons.h"
#include "../common/sokectUtils/socketUtil.h"


int serverSocket, serverLength;
struct sockaddr_un serverAddress;

/**
 * The function `creatConnection` creates a socket stream, sets up the server address, and connects to
 * the server.
 */
void creatConnection(){

    /* Creating Socket Stream */
    serverSocket = socket(AF_UNIX,SOCK_STREAM,0);
	if (serverSocket  < 0){
        printError(strerror(errno));
		printFatalError("client: can't open stream socket");
    }else
        printWarning("client: socket created.");

    /* Creating address*/
	serverAddress.sun_family = AF_UNIX;
	strcpy(serverAddress.sun_path, UNIXSTREAM_PATH);
    serverLength = sizeof(serverAddress);

    /* Connect to server */
    if ( connect(serverSocket, (struct sockaddr *)&serverAddress, serverLength) < 0)
		printFatalError("client: can not connect to server.");

}

/**
 * The closeConnection function closes the server socket.
 */
void closeConnection(){
    close(serverSocket);
}