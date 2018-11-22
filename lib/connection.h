/* 2017/11/14
 * version 1.0
 * */

#ifndef __CONNECTION_H__
#define __CONNECTION_H__

/* createTCPServerSocket(): allocates, binds and marks the server
 * socket as ready to accept incoming connections
 * @param port: which port to bind
 * @return: a socket
 * */
int createTCPServerSocket(unsigned short port);

/* createTCPClientSocket(): allocates a socket in client side
 * and connects to server
 * @param address: server address
 * @param port: server port where socket has been bound
 * @return: a socket
 * */
int createTCPClientSocket(char *address, unsigned short port);

/* acceptTCPConnection(): blocks until a valid connection is establish and
 * returns the socket descriptor for that connection
 * @param serverSockfd: socket descriptor in server
 * @return: a socket descriptor
 * */
int acceptTCPConnection(int serverSockfd);

#endif
