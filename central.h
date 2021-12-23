#include "common.h"
#include <iomanip>
#include <iostream>
#include <utility>
#include <vector>
#include <string>
#include <algorithm>
#include <sstream>

#define serverName "Central Server"

socketDetails setupUDPSocketAddress(const char *port) {
    struct addrinfo hints;
    struct addrinfo *res;
    int sockfd;
    int status;

    // Set UDP Socket Parameters
    memset(&hints, 0, sizeof hints); // Make sure hints is empty
    hints.ai_family = AF_INET;       // Use IPv4
    hints.ai_socktype = SOCK_DGRAM;  // Use TCP sockets

    // Checking successful assignment of address information
    if ((status = getaddrinfo(LOCALHOST, port, &hints, &res)) != 0) {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        exit(1);
    }

    socketDetails sockDet;
    sockDet.res = res;
    sockDet.sockfd = sockfd;
    return sockDet;
}


void getGraphFromServerT(int sockfd, char usernameA[], char usernameB[], int *&nodeAMapPtr, int *&nodeBMapPtr, int *&nodeCount, int *&nodeListPtr, int **&adjacencyMatrixPtr) {
    
    // After writing this part of code, it would be clear this will mostly be repeated everywhere, so created generic function
    socketDetails serverTCentralSocket = setupUDPSocketAddress(SERVER_T_UDP_PORT);

    // Send A's username to Server T
    int numbytes;
    char error[50];
    if ((numbytes = sendto(sockfd, usernameA, strlen(usernameA), 0, serverTCentralSocket.res->ai_addr, serverTCentralSocket.res->ai_addrlen)) == -1) {
        sprintf(error, "%s - Error in sending username for Client A", serverName);
        perror(error);
        exit(1);
    }

    // Send B's username to Server T
    if ((numbytes = sendto(sockfd, usernameB, strlen(usernameB), 0, serverTCentralSocket.res->ai_addr, serverTCentralSocket.res->ai_addrlen)) == -1) {
        sprintf(error, "%s - Error in sending username for Client B", serverName);
        perror(error);
        exit(1);
    }
    printf("The Central server sent a request to Backend-Server T.\n");

    int *nodeCountPtr = new int(0);
    // Receiving node count from Server T
    if ((numbytes = recvfrom(sockfd, nodeCountPtr, sizeof(int), 0, serverTCentralSocket.res->ai_addr, &serverTCentralSocket.res->ai_addrlen)) == -1) {
        sprintf(error, "%s - Error in receiving node count", serverName);
        perror(error);
        exit(1);
    }

    int nodeListSize = *nodeCountPtr;
    int *nodeList = new int[nodeListSize];

    // Receiving node list size from Server T
    if ((numbytes = recvfrom(sockfd, nodeList, nodeListSize * sizeof(int), 0, serverTCentralSocket.res->ai_addr, &serverTCentralSocket.res->ai_addrlen)) == -1) {
        sprintf(error, "%s - Error in receiving node list", serverName);
        perror(error);
        exit(1);
    }

    int **adjacencyMatrix = new int *[nodeListSize];
    // Receiving adjacency matrix from Server T in looping manner
    for (int i = 0; i < nodeListSize; i++) {
        adjacencyMatrix[i] = new int[nodeListSize];
        if ((numbytes = recvfrom(sockfd, adjacencyMatrix[i], nodeListSize * sizeof(int), 0, serverTCentralSocket.res->ai_addr, &serverTCentralSocket.res->ai_addrlen)) == -1) {
            sprintf(error, "%s - Error in receiving adjacency matrix", serverName);
            perror(error);
            exit(1);
        }
    }

    int *nodeAMap = new int(0);
    // Receiving mapping for Node A
    if ((numbytes = recvfrom(sockfd, nodeAMap, sizeof(int), 0, serverTCentralSocket.res->ai_addr, &serverTCentralSocket.res->ai_addrlen)) == -1) {
        sprintf(error, "%s - Error in receiving node A mapping", serverName);
        perror(error);
        exit(1);
    }

    int *nodeBMap = new int(0);
    // Receiving mapping for Node A
    if ((numbytes = recvfrom(sockfd, nodeBMap, sizeof(int), 0, serverTCentralSocket.res->ai_addr, &serverTCentralSocket.res->ai_addrlen)) == -1) {
        sprintf(error, "%s - Error in receiving node A mapping", serverName);
        perror(error);
        exit(1);
    }
    
    // Reassigning pointers for transferring control to main function
    nodeCount = nodeCountPtr;
    nodeListPtr = nodeList;
    adjacencyMatrixPtr = adjacencyMatrix;
    nodeAMapPtr = nodeAMap;
    nodeBMapPtr = nodeBMap;

    freeaddrinfo(serverTCentralSocket.res);
}

void getScoresFromServerS(int sockfd, int *&nodeCount, int *&nodeListPtr, int *&scoreList, std::vector<std::string> &nameList) {
    socketDetails serverSCentralSocket = setupUDPSocketAddress(SERVER_S_UDP_PORT);
    
    int *nodeCountPtr = nodeCount;
    int nodeListSize = *nodeCountPtr;
    int *nodeList = nodeListPtr;
    int numbytes;
    char error[50];
    
    // Sending node count to Server S
    if ((numbytes = sendto(sockfd, nodeCountPtr, sizeof(int), 0, serverSCentralSocket.res->ai_addr, serverSCentralSocket.res->ai_addrlen)) == -1) {
        sprintf(error, "%s - Error in sending node count", serverName);
        perror(error);
        exit(1);
    }

    // Sending node list received from T to Server S
    if ((numbytes = sendto(sockfd, nodeList, nodeListSize * sizeof(int), 0, serverSCentralSocket.res->ai_addr, serverSCentralSocket.res->ai_addrlen)) == -1) {
        sprintf(error, "%s - Error in sending node list", serverName);
        perror(error);
        exit(1);
    }
    printf("The Central server sent a request to Backend-Server S.\n");

    int *scoresListPtr = new int[nodeListSize];
    // Receiving Score list from server S
    if ((numbytes = recvfrom(sockfd, scoresListPtr, nodeListSize * sizeof(int), 0, serverSCentralSocket.res->ai_addr, &serverSCentralSocket.res->ai_addrlen)) == -1) {
        sprintf(error, "%s - Error in receiving score list", serverName);
        perror(error);
        exit(1);
    }

    // Receiving score list from server S for intermediate nodes
    for (int i = 0; i < nodeListSize; i++) {
        char name[512];
        if ((numbytes = recvfrom(sockfd, name, sizeof name, 0, serverSCentralSocket.res->ai_addr, &serverSCentralSocket.res->ai_addrlen)) == -1) {
            sprintf(error, "%s - Error in receiving intermediate name list score", serverName);
            perror(error);
            exit(1);
        }
        nameList.push_back(name);
    }

    // Reassigning pointers for transferring control to main function
    scoreList = scoresListPtr;
    freeaddrinfo(serverSCentralSocket.res);
}

void getCompatibilityFromServerP(int sockfd, int *&nodeCount, int *&scoreList, int **&adjacencyMatrixPtr, int *&nodeAMapPtr,
                             int *&nodeBMapPtr, std::vector<std::string> &nameList, int *&stringSizePtr, std::string &clientAString, std::string &clientBString) {
    socketDetails serverPCentralSocket = setupUDPSocketAddress(SERVER_P_UDP_PORT);

    int *nodeCountPtr = nodeCount;
    int *scoresListPtr = scoreList;
    int **adjacencyMatrix = adjacencyMatrixPtr;
    int *nodeAMap = nodeAMapPtr;
    int *nodeBMap = nodeBMapPtr;
    int nodeListSize = *nodeCountPtr;

    // Send node list size
    int numbytes;
    char error[50];

    // Send node count to Server P for processing
    if ((numbytes = sendto(sockfd, nodeCountPtr, sizeof(int), 0, serverPCentralSocket.res->ai_addr, serverPCentralSocket.res->ai_addrlen)) == -1) {
        sprintf(error, "%s - Error in sending node count to server P", serverName);
        perror(error);
        exit(1);
    }

    // Send scores received from Server S
    if ((numbytes = sendto(sockfd, scoresListPtr, nodeListSize * sizeof(int), 0, serverPCentralSocket.res->ai_addr, serverPCentralSocket.res->ai_addrlen)) == -1) {
        sprintf(error, "%s - Error in sending scores list", serverName);
        perror(error);
        exit(1);
    }

    // Send adjacency matrix received from Server T
    for (int i = 0; i < nodeListSize; i++) {
        if ((numbytes = sendto(sockfd, adjacencyMatrix[i], nodeListSize * sizeof(int), 0, serverPCentralSocket.res->ai_addr, serverPCentralSocket.res->ai_addrlen)) == -1) {
            sprintf(error, "%s - Error in sending adjacenency matrix", serverName);
            perror(error);
            exit(1);
        }
    }

    // Send mappings to node A calculated by Server T
    if ((numbytes = sendto(sockfd, nodeAMap, sizeof(int), 0, serverPCentralSocket.res->ai_addr, serverPCentralSocket.res->ai_addrlen)) == -1) {
        sprintf(error, "%s - Error in sending node A mapping", serverName);
        perror(error);
        exit(1);
    }

    // Send mappings to node B calculated by Server T
    if ((numbytes = sendto(sockfd, nodeBMap, sizeof(int), 0, serverPCentralSocket.res->ai_addr, serverPCentralSocket.res->ai_addrlen)) == -1) {
        sprintf(error, "%s - Error in sending node B mapping", serverName);
        perror(error);
        exit(1);
    }

    // Send name list from Server T file read
    for (int i = 0; i < nameList.size(); i++) {
        const char *temp = nameList[i].c_str();
        if ((numbytes = sendto(sockfd, temp, (nameList[i].length() + 1), 0, serverPCentralSocket.res->ai_addr, serverPCentralSocket.res->ai_addrlen)) == -1) {
            sprintf(error, "%s - Error in sending name list", serverName);
            perror(error);
            exit(1);
        }
    }
    printf("The Central server sent a processing request to Backend-Server P.\n");

    int *stringLength = new int(0);
    // Receive output string length
    if ((numbytes = recvfrom(sockfd, stringLength, sizeof(int), 0, serverPCentralSocket.res->ai_addr, &serverPCentralSocket.res->ai_addrlen)) == -1) {
        sprintf(error, "%s - Error in receiving output string length", serverName);
        perror(error);
        exit(1);
    }

    int stringSize = *stringLength;
    char clientAStr[MAXIMUM_BUFFER];
    // Receive output string for Client A
    if ((numbytes = recvfrom(sockfd, clientAStr, sizeof clientAStr, 0, serverPCentralSocket.res->ai_addr, &serverPCentralSocket.res->ai_addrlen)) == -1) {
        sprintf(error, "%s - Error in receiving output string for Client A", serverName);
        perror(error);
        exit(1);
    }
    clientAString.append(clientAStr);

    char clientBStr[MAXIMUM_BUFFER];
    // Receive output string for Client B
    if ((numbytes = recvfrom(sockfd, clientBStr, sizeof clientBStr, 0, serverPCentralSocket.res->ai_addr, &serverPCentralSocket.res->ai_addrlen)) == -1) {
        sprintf(error, "%s - Error in receiving output string for Client B", serverName);
        perror(error);
        exit(1);
    }
    clientBString.append(clientBStr);
    stringSizePtr = stringLength;
    freeaddrinfo(serverPCentralSocket.res);
}