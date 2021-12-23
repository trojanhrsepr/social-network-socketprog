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

#define serverName "Client A"

/**
 * @brief 
 * Class for Client A will be symmetric to that of B. This takes input as single edge
 * for whom distance needs to be found.
 * @param argc 
 * @param argv 
 * @return int 
 */
int main(int argc, char *argv[]) {
    int numbytes;
    char error[50];
    serverStartMessage(serverName);

    // Client A socket will be created and connection will be established without binding
    // Code will go into else part for this function and hence, Port binding does not happen
    socketDetails clientASocket = createTCPSocket(CENTRAL_TCP_CLIENT_A_PORT, serverName);

    // Sending user name for Client A to Central server
    if ((numbytes = send(clientASocket.sockfd, argv[1], strlen(argv[1]), 0)) == -1) {
        sprintf(error, "%s - Error in sending data", serverName);
        perror(error);
        exit(1);
    }

    // Send successful for Client A    
    printf("Client A sent %s to the Central server.\n", argv[1]);

    // Receive size for Client A
    int *outputSize = new int(0);
    if ((numbytes = recv(clientASocket.sockfd, outputSize, sizeof(int), 0)) == -1) {
        sprintf(error, "%s - Error in receiving data", serverName);
        perror(error);
        exit(1);
    }

    // Receive output string for Client A
    char clientAReceive[MAXIMUM_BUFFER];
    if ((numbytes = recv(clientASocket.sockfd, clientAReceive, sizeof clientAReceive, 0)) == -1) {
        sprintf(error, "%s - Error in receiving data", serverName);
        perror(error);
        exit(1);
    }
    printf("Output: %s",clientAReceive);
    
    close(clientASocket.sockfd);
}