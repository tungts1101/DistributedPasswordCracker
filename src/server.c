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
#include "../lib/structure.h"

void *ThreadRecv(void *threadArgs);
void *ThreadSend(void *threadArgs);

struct ThreadArgs {
    int clntSock;
};

int listenfd;
int requestNo = 0;

int main (int argc, char **argv) {
	pthread_t threadID;
	struct ThreadArgs *threadArgs;
	int listenfd, connfd;

	// init all data structure
	init();

	listenfd = createTCPServerSocket(SERV_PORT);

	// spawn a thread for sending job purpose
	pthread_create(&threadID, NULL, ThreadSend, NULL);

	while(1) {
		connfd = acceptTCPConnection(listenfd);

		// printf("%s%d\n","Received request from ", connfd);
		threadArgs = (struct ThreadArgs *)malloc(sizeof(struct ThreadArgs));
		threadArgs -> clntSock = connfd;
		
		pthread_create(&threadID, NULL, ThreadRecv, (void *) threadArgs);
		// printf("with thread %ld\n", (long int) threadID);
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
	Connection conn;
	int requesterID;
	int sockfd;

	while((n = recv(clientSock, (struct Message*)&req, sizeof req, 0)) > 0) {
		switch(req.command) {
			case HASH: ;
				// printf("Message info: %d %d %s\n",req.clientID, req.requestID, req.other);
				
				// unsigned int requestID = getNewRequestID();
				// addRequest(requestID);

				unsigned int requestID = ++requestNo;
				Request request = {requestID, *req.other};
				splitJob(request);

				if(req.clientID == 0) {
					clientID = getNewClientID();
					printf("new client = %u\n", clientID);

					setConnection(&conn, clientID, clientSock);
					addConnection(conn);
					printConnection();

					Requester requester = {clientID, {0, ""}};
					addRequester(requester);
					
					addRequestToRequester(requester.clientID, request);
					printRequesterList();	
				} else {
					addRequestToRequester(req.clientID, request);
					printRequesterList();
				}
				
				res = response(ACCEPT, clientID, requestID, "");
				send(clientSock, (struct Message *)&res, sizeof res, 0);
				
				break;

			case JOIN:
				// printf("Message info: %d %d\n",req.clientID, req.requestID);

				if(req.clientID == 0) {
					clientID = getNewClientID();
					printf("new client = %u\n", clientID);
					
					setConnection(&conn, clientID, clientSock);
					addConnection(conn);
					printConnection();

					Worker worker = {clientID, 0};
					addWorkerList(worker);
					printWorkerList();

					res = response(ACCEPT, clientID, 0, "");
					send(clientSock, (struct Message *)&res, sizeof res, 0);
				}
				
				break;

			case DONE_NOT_FOUND:
				printf("Client %d cannot found job = %d\n", req.clientID, req.requestID);

				requesterID = getRequesterFromRequest(req.requestID);
				sockfd = getSocketDesc(requesterID);

				res = response(DONE_NOT_FOUND, requesterID, req.requestID, "aasNLphgV1W3o");
				send(sockfd, (struct Message *)&res, sizeof res, 0);

				break;
			
			case DONE_FOUND:
				printf("Client %d found job = %d\n", req.clientID, req.requestID);
				printf("Password = %s\n", req.other);
				
				strcpy(other, "aasNLphgV1W3o");
				strcat(other, " ");
				strcat(other, req.other);
				
				requesterID = getRequesterFromRequest(req.requestID);
				sockfd = getSocketDesc(requesterID);

				res = response(DONE_FOUND, requesterID, req.requestID, other);
				send(sockfd, (struct Message *)&res, sizeof res, 0);
				break;

			default:
				break;
		}
	}
}

char *getMsgFromJob(Job job) {
	char *hash = getHashFromRequest(job.requestID);

	char tmp[3];
	sprintf(tmp, "%d", job.package);

	char *other = malloc(MSG_OTHER_LENGTH);

	strcat(other, hash);
	strcat(other, " ");
	strcat(other, tmp);

	return other;
}

void*ThreadSend(void *threadArgs) {
	pthread_detach(pthread_self());

	struct Message res;
	char *other = malloc(MSG_OTHER_LENGTH);
	unsigned int workerID;
	int sockfd;
	int jobPos;
	int workerPos;

	while(1) {
		jobPos = getFirstJob();
		if(jobPos != -1) {	// check if we have a job
			workerPos = getFirstWorker();
			if(workerPos != -1) {	// check if we have a worker
				printf("worker %d, job %d\n", workerPos, jobPos);
				memset(&other, 0, sizeof other);
				other = getMsgFromJob(jobList[jobPos]);

				printf("other = %s\n", other);

				sockfd = getSocketDesc(workerList[workerPos].clientID);

				if(sockfd != 0) {
					res = response(JOB, workerList[workerPos].clientID, jobList[jobPos].requestID, other);
				
					send(sockfd, (struct Message*)&res, sizeof res, 0);
				}

				jobList[jobPos].workerID = workerList[workerPos].clientID;	// assign job
				workerList[workerPos].jobNumber++;	// increase job number	
				printWorkerList();
				// printjobList();
			}
		}
		sleep(5);	// after interval
	}
}