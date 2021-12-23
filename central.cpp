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
#include "central.h"

// Inherited from parent header file
//#define serverName "Central Server"

int main()
{
    int child_fd_A;
    int child_fd_B;
    char error[50];
    int numbytes;
    char usernameA[512];
    char usernameB[512];
    int *usernameAMap;
    int *usernameBMap;
    int *nodeCount;
    int *nodeList;
    int **adjacencyMatrix;
    
    serverStartMessage(serverName);
    socketDetails socketA = createTCPSocket(CENTRAL_TCP_CLIENT_A_PORT, serverName);
    socketDetails socketB = createTCPSocket(CENTRAL_TCP_CLIENT_B_PORT, serverName);
    socketDetails socketUDP = createUDPSocket(CENTRAL_UDP_PORT, serverName);

    struct sockaddr_in client_addr_A, client_addr_B;
    socklen_t addr_len = sizeof(client_addr_A);

    // All receive commands have been split into main and all send except for client have been added to header
    while (1) {
        // Accept for Client A and creation of Child socket
        child_fd_A = accept(socketA.sockfd, (struct sockaddr *)&client_addr_A, &addr_len);
        if (child_fd_A == -1) {
            sprintf(error, "%s - Error in accept for Client A", serverName);
            perror(error);
            exit(1);
        }

        // Receive username for Client A
        if ((numbytes = recv(child_fd_A, usernameA, sizeof usernameA, 0)) == -1) {
            sprintf(error, "%s - Error in receiving username for Client A", serverName);
            perror(error);
            exit(1);
        }
        usernameA[numbytes] = '\0';
        printf("The Central server received input=%s from the client using TCP over port %s.\n", usernameA, CENTRAL_TCP_CLIENT_A_PORT);

        // Accept for Client B and creation of Child socket
        child_fd_B = accept(socketB.sockfd, (struct sockaddr *)&client_addr_B, &addr_len);
        if (child_fd_B == -1) {
            sprintf(error, "%s - Error in accept for Client B", serverName);
            perror(error);
            exit(1);
        }
        
        // Receive username for Client A
        if ((numbytes = recv(child_fd_B, usernameB, sizeof usernameB, 0)) == -1) {
            sprintf(error, "%s - Error in receiving username for Client B", serverName);
            perror(error);
            exit(1);
        }
        usernameB[numbytes] = '\0';
        printf("The Central server received input=%s from the client using TCP over port %s.\n", usernameB, CENTRAL_TCP_CLIENT_B_PORT);

        // Sending information to Server T and receiving back graph details
        getGraphFromServerT(socketUDP.sockfd, usernameA, usernameB, usernameAMap, usernameBMap, nodeCount, nodeList, adjacencyMatrix);
        printf("The Central server received information from Backend-Server T using UDP over port %s.\n", SERVER_T_UDP_PORT);

        int *scoreList;
        std::vector<std::string> nameList;

        // Sending information to Server T and getting back Score list
        getScoresFromServerS(socketUDP.sockfd, nodeCount, nodeList, scoreList, nameList);
        printf("The Central server received information from Backend-Server S using UDP over port %s.\n", SERVER_S_UDP_PORT);

        int *outputSize;
        std::string clientAString;
        std::string clientBString;
        
        // Getting compatibility scores from Server P in response
        getCompatibilityFromServerP(socketUDP.sockfd, nodeCount, scoreList, adjacencyMatrix, usernameAMap, usernameBMap, nameList, outputSize, clientAString, clientBString);
        printf("The Central server received the results from backend server P using UDP over port %s.\n", SERVER_P_UDP_PORT);

        // Final results sent to client
        // Send Client A Output String length
        int clientStringSize = clientAString.length();
        int *clientAStrlen = new int(clientStringSize);
        if ((numbytes = send(child_fd_A, clientAStrlen, sizeof(int), 0)) == -1) {
            sprintf(error, "%s - Error in sending Output length Client A", serverName);
            perror(error);
            exit(1);
        }

        // Converting CPP to C format
        const char *clientStringA = clientAString.c_str();
        // Sending Client A Output
        if ((numbytes = send(child_fd_A, clientStringA, clientStringSize + 1, 0)) == -1){
            sprintf(error, "%s - Error in sending Output for Client A", serverName);
            perror(error);
            exit(1);
        }
        printf("The Central server sent the results to client A.\n");

        clientStringSize = clientBString.length();
        // Sending Client A Output String Length
        int *clientBStrlen = new int(clientStringSize);
        if ((numbytes = send(child_fd_B, clientBStrlen, sizeof(int), 0)) == -1) {
            sprintf(error, "%s - Error in sending Output length Client B", serverName);
            perror(error);
            exit(1);
        }

        // Converting CPP to C format
        const char *clientStringB = clientBString.c_str();

        // Sending output for Client B
        if ((numbytes = send(child_fd_B, clientStringB, clientStringSize + 1, 0)) == -1){
            sprintf(error, "%s - Error in sending Output for Client B", serverName);
            perror(error);
            exit(1);
        }
        printf("The Central server sent the results to client B.\n");

        close(child_fd_A);
        close(child_fd_B);
    }
}