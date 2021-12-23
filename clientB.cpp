#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include "common.h"

#define serverName "Client B"

/**
 * @brief 
 * Client B is symmetric to Client A and takes in a single input for finding compatibility 
 * with input of A
 * @param argc 
 * @param argv 
 * @return int 
 */
int main(int argc, char *argv[]) {
    int numbytes;
    char error[50];
    serverStartMessage(serverName);

    // Client B socket will be created and connection will be established without binding
    // Code will go into else part for this function and hence, Port binding does not happen
    socketDetails clientBSocket = createTCPSocket(CENTRAL_TCP_CLIENT_B_PORT, serverName);

    // Established TCP connection, send user A name to Central server
    if ((numbytes = send(clientBSocket.sockfd, argv[1], strlen(argv[1]), 0)) == -1){
        sprintf(error, "%s - Error in sending data", serverName);
        perror(error);
        exit(1);
    }

    // Send successful for Client B
    printf("Client B sent %s to the Central server.\n",argv[1]);
    
    // Receive size for Client B
    int *outputSize = new int(0);
    if ((numbytes = recv(clientBSocket.sockfd, outputSize, sizeof(int), 0)) == -1) {
        sprintf(error, "%s - Error in receiving data", serverName);
        perror(error);
        exit(1);
    }

    // Receive output string for Client B
    char clientBReceive[MAXIMUM_BUFFER];
    if ((numbytes = recv(clientBSocket.sockfd, clientBReceive, sizeof clientBReceive, 0)) == -1) {
        sprintf(error, "%s - Error in receiving data", serverName);
        perror(error);
        exit(1);
    }
    printf("Output: %s",clientBReceive);

    close(clientBSocket.sockfd);
}