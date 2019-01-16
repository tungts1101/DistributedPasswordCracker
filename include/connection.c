/* 2018/11/14
 * version 1.0
 * */

#include <sys/socket.h>     // socket(), bind(), connect()
#include <arpa/inet.h>      // sockaddr_in, inet_addr()
#include <string.h>         // memset()
#include <fcntl.h>
#include <errno.h>
#include "error.h"   // error()
#include "connection.h"
#include "config.h"

int createTCPServerSocket(unsigned short port) {
    int sockfd;
    struct sockaddr_in serverAddr;

    // create a socket using TCP
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        error(ERR_SOCK_FAILED);
    
    //preparation of the socket address
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY); // accept any incoming interface
    serverAddr.sin_port = htons(port);
    
    if(bind(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
        error(ERR_BIND_FAILED);
    
    if(listen(sockfd, MAX_PENDING) < 0)
        error(ERR_LIST_FAILED);
    
    return sockfd;
}

int createTCPClientSocket(char *address, unsigned short port) {
    int sockfd;
    struct sockaddr_in serverAddr;

    // create a socket using TCP
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        error(ERR_SOCK_FAILED);

    memset(&serverAddr, 0, sizeof(serverAddr));     // zero out structure
    serverAddr.sin_family = AF_INET;                // internet address family
    serverAddr.sin_addr.s_addr = inet_addr(address);// server ip address
    serverAddr.sin_port = htons(port);              // server port

    // establish the connection to the server
    if(connect(sockfd, (struct sockaddr *)&serverAddr, sizeof serverAddr) < 0)
        error(ERR_CONN_FAILED);

    return sockfd;
}

int acceptTCPConnection(int serverSockfd) {
    int clientSockfd;   // socket descriptor for connection
    struct sockaddr_in clientAddr;
    socklen_t clientLen = sizeof(clientAddr);

    if((clientSockfd = accept(serverSockfd, (struct sockaddr *)&clientAddr, &clientLen)) < 0)
        error(ERR_ACCE_FAILED);
    
    return clientSockfd;
}