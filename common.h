/**
 * @brief 
 * Common Header file created for utilities and constants used repeatedly 
 * across the project. Increases simplicity in changing parameters and 
 * checking for code changes, debugging or modifications.
 * 
 * This also allows to reduce code cluttering in the main function and 
 * provide more focus into understanding the code rather than its implementation
 */

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

#ifndef COMMON_H
#define COMMON_H

/**
 * @brief 
 * Input files
 */
#define scoreFile "scores.txt"
#define edgeFile "edgelist.txt"

/**
 * @brief 
 * Server ports
 */
#define SERVER_T_UDP_PORT "21057"
#define SERVER_S_UDP_PORT "22057"
#define SERVER_P_UDP_PORT "23057"

/**
 * @brief 
 * Central Server ports
 */
#define CENTRAL_UDP_PORT "24057"
#define CENTRAL_TCP_CLIENT_A_PORT "25057"
#define CENTRAL_TCP_CLIENT_B_PORT "26057"

/**
 * @brief 
 * Localhost definition
 */
#define LOCALHOST "127.0.0.1"
#define MAXIMUM_BUFFER 30000

/**
 * @brief 
 * Structure to store all socket details from function
 */
typedef struct socketDetails {
    struct addrinfo *res;
    int sockfd;
} socketDetails;

/**
 * @brief 
 * Generic server details message picked up from project description
 * @param server 
 * @param protocol 
 * @param port 
 */
void serverDetailsMessage(const char *server, char *protocol, char *port) {
    printf("The %s is up and running using %s on port %s\n", server, protocol, port);
}

/**
 * @brief 
 * Generic message indicating start of server
 * @param server 
 */
void serverStartMessage(char *server) {
    printf("The %s is up and running.\n", server);
}

/**
 * @brief 
 * Generic function to create a TCP Socket used across the project
 * 
 * Code reference for function from Beej's Guide to Socket Programming
 * Used same variables as Ch6 Pg35. Switched out generic commands for IPv4/v6 to IPv4 only
 * @param port 
 * @param server 
 * @return socketDetails
 */
socketDetails createTCPSocket(char *port, char *server)
{
    struct addrinfo hints;
    struct addrinfo *res;
    int sockfd;
    int status;
    char error[50];

    // Set TCP Socket Parameters
    memset(&hints, 0, sizeof hints); // Make sure hints is empty
    hints.ai_family = AF_INET;       // Use IPv4
    hints.ai_socktype = SOCK_STREAM; // Use TCP sockets

    // Checking successful assignment of address information
    if ((status = getaddrinfo(LOCALHOST, port, &hints, &res)) != 0) {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        exit(1);
    }

    // Create a socket
    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

    // Check if socket was created
    if (sockfd == -1) {
        sprintf(error, "%s - Error in socket creation", server);
        perror(error);
        exit(1);
    }

    // For Central server, bind and listen
    if(!strcmp(server, "Central Server")) {
        // Bind it to port and IP Address from status
        if (bind(sockfd, res->ai_addr, res->ai_addrlen) == -1) {
            close(sockfd);
            sprintf(error, "%s - Error in socket binding", server);
            perror(error);
            exit(1);
        }

        // All done with this structure
        freeaddrinfo(res);

        if (listen(sockfd, 5) == -1) {
            close(sockfd);
            sprintf(error, "%s - Error in socket listen", server);
            perror(error);
            exit(1);
        }
    }
    // For clients A and B
    else {
        // Directly connecting to central server without binding
        if (connect(sockfd, res->ai_addr, res->ai_addrlen) != 0) {
            close(sockfd);
            sprintf(error, "%s - Error in socket connection", server);
            perror(error);
            exit(1);
        }

        // All done with this structure
        freeaddrinfo(res);
    }

    // Message to indicate successful socket creation
    serverDetailsMessage(server, "TCP", port);
    socketDetails sockDet;
    sockDet.res = res;
    sockDet.sockfd = sockfd;
    return sockDet;
}

/**
 * @brief 
 * Generic function to create a UDP Socket used across the project
 * 
 * Code used has reference from Beej as well. Function definitions were kept similar 
 * to that of TCP.
 * @param port 
 * @param server 
 * @return socketDetails 
 */
socketDetails createUDPSocket(char *port, char *server)
{
    struct addrinfo hints;
    struct addrinfo *res;
    int sockfd;
    int status;
    char error[50];

    // Set UDP Socket Parameters
    memset(&hints, 0, sizeof hints); // Make sure hints is empty
    hints.ai_family = AF_INET;       // Use IPv4
    hints.ai_socktype = SOCK_DGRAM;  // Use TCP sockets

    // Checking successful assignment of address information
    if ((status = getaddrinfo(LOCALHOST, port, &hints, &res)) != 0)
    {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        exit(1);
    }

    // Create UDP Socket
    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

    // Socket not created
    if (sockfd == -1) {
        sprintf(error, "%s - Error in socket creation", server);
        perror(error);
        exit(1);
    }

    // Binding failed
    if (bind(sockfd, res->ai_addr, res->ai_addrlen) == -1) {
        close(sockfd);
        sprintf(error, "%s - Error in socket binding", server);
        perror(error);
        exit(1);
    }

    // All done with this structure
    freeaddrinfo(res);

    // Message to indicate successful socket creation
    serverDetailsMessage(server, "UDP", port);
    socketDetails sockDet;
    sockDet.res = res;
    sockDet.sockfd = sockfd;
    return sockDet;
}

#endif