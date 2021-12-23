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
#include "serverT.h"

#define MAXBUFLEN 512

#define serverName "Server T"

int main() {
    serverStartMessage(serverName);
    socketDetails serverTSocket = createUDPSocket(SERVER_T_UDP_PORT, serverName);

    // Create Name Score Map and resultant graph
    std::map<std::string, int> nameScoreMap = populateNameScoreMapFromFile();
    Graph *networkGraph = createGraphFromFile(nameScoreMap);

    struct sockaddr_in cliaddr;
    socklen_t addr_len = sizeof(cliaddr);
    int numbytes;
    char error[50];
    char usernameA[512];
    char usernameB[512];
    
    while (1) {
        // Receive Username for Client A from central server
        if ((numbytes = recvfrom(serverTSocket.sockfd, usernameA, MAXBUFLEN - 1, 0, (struct sockaddr *)&cliaddr, &addr_len)) == -1) {
            sprintf(error, "%s - Error in receiving username", serverName);
            perror(error);
            exit(1);
        }
        // Added null after checking online reference as to why string wasn't terminating properly
        usernameA[numbytes] = '\0';
        std::string nodeA(usernameA);

        // Receive Username for Client B from central server
        if ((numbytes = recvfrom(serverTSocket.sockfd, usernameB, MAXBUFLEN - 1, 0, (struct sockaddr *)&cliaddr, &addr_len)) == -1) {
            sprintf(error, "%s - Error in receiving username", serverName);
            perror(error);
            exit(1);
        }
        usernameB[numbytes] = '\0';
        std::string nodeB(usernameB);
        printf("The ServerT received a request from Central to get the topology.\n");

        // Combine total number of nodes and convert to integer array
        std::set<int> nodeCombinedList;
        std::vector<int> nodeAList = findReachableNodes(nodeA, networkGraph, nameScoreMap);
        std::vector<int> nodeBList = findReachableNodes(nodeB, networkGraph, nameScoreMap);   
        for (int i = 0; i < nodeAList.size(); i++) {
            nodeCombinedList.insert(nodeAList[i]);
        }
        for (int j = 0; j < nodeBList.size(); j++) {
            nodeCombinedList.insert(nodeBList[j]);
        }
        int *nodeArrayList = new int[nodeCombinedList.size()]; int i = 0;
        for (std::set<int>::iterator iterator = nodeCombinedList.begin(); iterator != nodeCombinedList.end(); iterator++) {
            nodeArrayList[i++] = *iterator;
        }

        // Send total number of nodes to Central server
        int nodeCombinedListSize = nodeCombinedList.size();
        int *nodeCount = new int(nodeCombinedListSize);
        if ((numbytes = sendto(serverTSocket.sockfd, nodeCount, sizeof(int), 0, (struct sockaddr *)&cliaddr, addr_len)) == -1) {
            sprintf(error, "%s - Error in sending total number of nodes", serverName);
            perror(error);
            exit(1);
        }

        // Sending node list to Central server
        if ((numbytes = sendto(serverTSocket.sockfd, nodeArrayList, nodeCombinedListSize * sizeof(int), 0, (struct sockaddr *)&cliaddr, addr_len)) == -1) {
            sprintf(error, "%s - Error in sending node list", serverName);
            perror(error);
            exit(1);
        }

        // Sending 2D Adjacency matrix to central server for processing
        int **adjacencyMatrix = convertVectorAdjecencyListToMatrix(networkGraph->adj, nodeArrayList, nodeCombinedListSize);
        for (int i = 0; i < nodeCombinedListSize; i++) {
            if ((numbytes = sendto(serverTSocket.sockfd, adjacencyMatrix[i], nodeCombinedListSize * sizeof(int), 0, (struct sockaddr *)&cliaddr, addr_len)) == -1) {
                sprintf(error, "%s - Error in sending adjacency matrix", serverName);
                perror(error);
                exit(1);
            }
        }

        // Sending map for Node A
        int *nodeAMap = new int(searchForIdNodesList(nodeArrayList, 0, (nodeCombinedListSize - 1), nameScoreMap[nodeA]));
        if ((numbytes = sendto(serverTSocket.sockfd, nodeAMap, sizeof(int), 0, (struct sockaddr *)&cliaddr, addr_len)) == -1) {
            sprintf(error, "%s - Error in sending map for Node A", serverName);
            perror(error);
            exit(1);
        }

        // Sending map for Node B
        int *nodeBMap = new int(searchForIdNodesList(nodeArrayList, 0, (nodeCombinedListSize - 1), nameScoreMap[nodeB]));
        if ((numbytes = sendto(serverTSocket.sockfd, nodeBMap, sizeof(int), 0, (struct sockaddr *)&cliaddr, addr_len)) == -1) {
            sprintf(error, "%s - Error in sending map for Node B", serverName);
            perror(error);
            exit(1);
        }
        printf("The ServerT finished sending the topology to Central.\n");
    }
    close(serverTSocket.sockfd);
}