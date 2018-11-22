/* 2018/11/13
 * version 1.0
 * */

#include <stdlib.h> 
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h> // recv()
#include <unistd.h>     // close()
#include <pthread.h>    // POSIX thread
#include "../lib/message.h"
#include "../lib/connection.h"
#include "../lib/config.h"
#include "../lib/error.h"
#include "../lib/other.h"
#include "../lib/structure.h"

void *ThreadRecv(void *threadArgs);
void *ThreadSend(void *threadArgs);

struct ThreadArgs {
    int clntSock;
};

int listenfd;

int main (int argc, char **argv) {
      pthread_t threadID;
      struct ThreadArgs *threadArgs;
      int listenfd, connfd;

      initConnection();
      printConnection();

      listenfd = createTCPServerSocket(SERV_PORT);

      pthread_create(&threadID, NULL, ThreadSend, NULL);

      while(1) {
            connfd = acceptTCPConnection(listenfd);

            printf("%s%d\n","Received request from ", connfd);
            threadArgs = (struct ThreadArgs *)malloc(sizeof(struct ThreadArgs));
            threadArgs -> clntSock = connfd;
            
            pthread_create(&threadID, NULL, ThreadRecv, (void *) threadArgs);
            printf("with thread %ld\n", (long int) threadID);
      }

      return 0;
}

void *ThreadRecv(void *threadArgs) {
      int clientSock;

      pthread_detach(pthread_self());
      clientSock = ((struct ThreadArgs *) threadArgs) -> clntSock;

      // handle logic when receving message
      int n;
      struct Message req;
      struct Message res;
      char *other = (char *)malloc(MSG_OTHER_LENGTH);
      int clientID;
      struct Connection conn;

      while((n = recv(clientSock, (struct Message*)&req, sizeof req, 0)) > 0) {
            switch(req.command) {
                  case HASH:
                        printf("Message info: %d %d %s\n",req.clientID,   req.requestID, req.other);
                        
                        if(req.clientID == 0) {
                              clientID = getNewClientID();
                              printf("new client = %u\n", clientID);

                              conn = createConnection(clientID, clientSock);
                              addConnection(conn);
                              printConnection();

                              res = response(ACCEPT, clientID, 0, "");
                              send(clientSock, (struct Message *)&res, sizeof res, 0);
                        }
                        
                        break;

                  case JOIN:
                        printf("Message info: %d %d\n",req.clientID, req.requestID);

                        if(req.clientID == 0) {
                              clientID = getNewClientID();
                              printf("new client = %u\n", clientID);
                              
                              conn = createConnection(clientID, clientSock);
                              addConnection(conn);
                              printConnection();

                              res = response(ACCEPT, clientID, 0, "");
                              send(clientSock, (struct Message *)&res, sizeof res, 0);
                        }
                        
                        break;

                  case DONE_NOT_FOUND:
                        printf("Client %d cannot found job = %d\n", req.clientID, req.requestID);

                        res = response(DONE_NOT_FOUND, 1, 1, "aasNLphgV1W3o");
                        send(4, (struct Message *)&res, sizeof res, 0);

                        break;
                  
                  case DONE_FOUND:
                        printf("Client %d found job = %d\n", req.clientID, req.requestID);
                        printf("Password = %s\n", req.other);
                        
                        strcpy(other, "aasNLphgV1W3o");
                        strcat(other, " ");
                        strcat(other, req.other);
                        
                        res = response(DONE_FOUND, 1, 1, other);
                        send(4, (struct Message *)&res, sizeof res, 0); // example of sending done_found_job to first connected request client
                        // in here it means requestCliID = 1, and clientSocket = 4
                        break;

                  default:
                        break;
            }
      }
}

void*ThreadSend(void *threadArgs) {
      pthread_detach(pthread_self());

      unsigned int count = 1;
      struct Message res;
      char *other = malloc(MSG_OTHER_LENGTH);

      // while(1) {
      //       other = getJob("aasNLphgV1W3o", count);
      //       printf("other = %s\n", other);

      //       res = response(JOB, 2, 1, other);
      //       send(5, (struct Message*)&res, sizeof res, 0);

      //       count++;
      //       memset(&other, 0, sizeof other);

      //       if(count > 3) count = 1;
      //       sleep(5);
      // }
}