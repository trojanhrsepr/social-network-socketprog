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
#include "serverS.h"

#define serverName "Server S"

int main()
{
    serverStartMessage(serverName);
    struct sockaddr_in cliaddr;
    socklen_t addr_len = sizeof(cliaddr);
    int numbytes;
    char error[50];
    
    socketDetails serverSSocket = createUDPSocket(SERVER_S_UDP_PORT, serverName);
    
    // Populate map from file
    Scores scoreMap;
    scoreMap = populateScoreFromFile(scoreMap);
    
    /* Removed as Piazza files displayed too many values
    std::cout<<"Names found in file:\n";
    for (int i = 0; i < scoreMap.nameVector.size(); i++)
        std::cout<<scoreMap.nameVector[i]<<"\n";
    */

    while (1) {
        // Receive node count from Central
        int *nodeCount = new int(0);
        if ((numbytes = recvfrom(serverSSocket.sockfd, nodeCount, sizeof(int), 0, (struct sockaddr *)&cliaddr, &addr_len)) == -1) {
            sprintf(error, "%s - Error in receiving data", serverName);
            perror(error);
            exit(1);
        }

        // Receive array based on node count received
        int *nodeArray = new int[*nodeCount];
        if ((numbytes = recvfrom(serverSSocket.sockfd, nodeArray, *nodeCount * sizeof(int), 0, (struct sockaddr *)&cliaddr, &addr_len)) == -1) {
            sprintf(error, "%s - Error in receiving data", serverName);
            perror(error);
            exit(1);
        }
        printf("The ServerS received a request from Central to get the scores.\n");

        // Sending scores on request
        int *scoreList = getScoreMatches(scoreMap.scoreVector, nodeArray, *nodeCount);
        if ((numbytes = sendto(serverSSocket.sockfd, scoreList, *nodeCount * sizeof(int), 0, (struct sockaddr *)&cliaddr, addr_len)) == -1) {
            sprintf(error, "%s - Error in sending scores", serverName);
            perror(error);
            exit(1);
        }
        printf("The ServerS finished sending the scores to Central.\n");

        // Processing intermediate nodes and sending name list
        std::vector<std::string> nameList = getNameMatches(scoreMap.nameVector, nodeArray, *nodeCount);
        // Parse names one by one and send them to avoid use of vectors. Vectors can't be sent through sockets
        for (int i = 0; i < *nodeCount; i++) {
            const char* temp = nameList[i].c_str();
            if ((numbytes = sendto(serverSSocket.sockfd, temp, (nameList[i].length() + 1), 0, (struct sockaddr *)&cliaddr, addr_len)) == -1) {
                sprintf(error, "%s - Error in sending names", serverName);
                perror(error);
                exit(1);
            }
        }
    }
    close(serverSSocket.sockfd);
}