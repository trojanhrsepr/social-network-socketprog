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
#include <float.h>
#include "common.h"
#include "serverP.h"

#define serverName "Server P"

int main() {
    serverStartMessage(serverName);
    socketDetails serverPSocket = createUDPSocket(SERVER_P_UDP_PORT, serverName);

    struct sockaddr_in cliaddr;
    socklen_t addr_len = sizeof(cliaddr);
    int numbytes;
    char error[50];

    while (1) {
        // Receive node count from central
        int *nodeCount = new int(0);
        if ((numbytes = recvfrom(serverPSocket.sockfd, nodeCount, sizeof(int), 0, (struct sockaddr *)&cliaddr, &addr_len)) == -1) {
            sprintf(error, "%s - Error in receiving data for node count", serverName);
            perror(error);
            exit(1);
        }

        // Receive node list from central
        int nodeListCount = *nodeCount;
        int *scoresList = new int[nodeListCount];
        if ((numbytes = recvfrom(serverPSocket.sockfd, scoresList, nodeListCount * sizeof(int), 0, (struct sockaddr *)&cliaddr, &addr_len)) == -1) {
            sprintf(error, "%s - Error in receiving score list", serverName);
            perror(error);
            exit(1);
        }
        
        // Receive adjacency matrix from central
        int **adjacencyMatrix = new int *[nodeListCount];
        for (int i = 0; i < nodeListCount; i++) {
            adjacencyMatrix[i] = new int[nodeListCount];
            if ((numbytes = recvfrom(serverPSocket.sockfd, adjacencyMatrix[i], nodeListCount * sizeof(int), 0, (struct sockaddr *)&cliaddr, &addr_len)) == -1) {
                sprintf(error, "%s - Error in receiving matrix", serverName);
                perror(error);
                exit(1);
            }
        }

        // Receiving client A mapping from central
        int *nodeAMap = new int(0);
        if ((numbytes = recvfrom(serverPSocket.sockfd, nodeAMap, sizeof(int), 0, (struct sockaddr *)&cliaddr, &addr_len)) == -1) {
            sprintf(error, "%s - Error in receiving Client A map", serverName);
            perror(error);
            exit(1);
        }

        // Receiving client B mapping from central
        // ToDo: Group both into single receive instance 
        int *nodeBMap = new int(0);
        if ((numbytes = recvfrom(serverPSocket.sockfd, nodeBMap, sizeof(int), 0, (struct sockaddr *)&cliaddr, &addr_len)) == -1) {
            sprintf(error, "%s - Error in receiving Client B map", serverName);
            perror(error);
            exit(1);
        }

        // Receive name list from central
        std::vector<std::string> namesList;
        for (int i = 0; i < nodeListCount; i++) {
            char name[512];
            if ((numbytes = recvfrom(serverPSocket.sockfd, name, sizeof name, 0, (struct sockaddr *)&cliaddr, &addr_len)) == -1) {
                sprintf(error, "%s - Error in receiving name list", serverName);
                perror(error);
                exit(1);
            }
            name[numbytes] = '\0';
            namesList.push_back(name);
        }
        printf("The ServerP received the topology and score information.\n");

        // Process the data now that all information is available
        double **weightAdjacencyMatrix = createAdjacencyMatrix(nodeListCount, scoresList, adjacencyMatrix);

        // Returns a pair of strings where the first string is the output message for client A and the second string is the output message for client B
        std::pair<std::string, std::string> finalResult = findCompatibleMatch(nodeListCount, weightAdjacencyMatrix, namesList, *nodeAMap, *nodeBMap);        
        std::cout << "Compatable match generated through Dijkstra:\n" 
                << finalResult.first<< " " << finalResult.second<< "\n";
        
        int numbytes;
        int stringSize = finalResult.first.length();
        int *stringLength = new int(stringSize);

        // Send result data length to Central
        if ((numbytes = sendto(serverPSocket.sockfd, stringLength, sizeof(int), 0, (struct sockaddr *)&cliaddr, addr_len)) == -1) {
            sprintf(error, "%s - Error in sending o/p length", serverName);
            perror(error);
            exit(1);
        }

        // Send Compatible for Client A
        const char *clientAOutput = finalResult.first.c_str();
        if ((numbytes = sendto(serverPSocket.sockfd, clientAOutput, stringSize + 1, 0, (struct sockaddr *)&cliaddr, addr_len)) == -1) {
            sprintf(error, "%s - Error in sending Client A o/p", serverName);
            perror(error);
            exit(1);
        }

        // Send Compatible for Client B 
        const char *clientBOutput = finalResult.second.c_str();
        if ((numbytes = sendto(serverPSocket.sockfd, clientBOutput, stringSize + 1, 0, (struct sockaddr *)&cliaddr, addr_len)) == -1) {
            sprintf(error, "%s - Error in sending Client B o/p", serverName);
            perror(error);
            exit(1);
        }
        printf("The ServerP finished sending the results to the Central.\n");
    }
    close(serverPSocket.sockfd);
}