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
int requestNo = 0;	// keep track of number of requests

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
	struct Message req, res;

	Connection conn = {0, 0};
	Requester requester = {0, {0, ""}};
	Worker worker = {0, 0};
	Request request = {0, ""};

	int clientID;
	char *other = malloc(MSG_OTHER_LENGTH);
	char *hash = malloc(HASH_LENGTH);
	char *password = malloc(PW_LENGTH);
	unsigned int package;
	int sockfd;

	int n;
	while((n = recv(clientSock, (struct Message*)&req, sizeof req, 0)) > 0) {
		switch(req.command) {
			case HASH:
				request.requestID = ++requestNo;
				strcpy(request.hash, req.other);

				clientID = req.clientID;
				if(clientID == 0) {
					clientID = getNewClientID();
					setConnection(&conn, clientID, clientSock);
					addConnection(conn);
				}
				
				res = response(ACCEPT, clientID, request.requestID, "");
				send(clientSock, (struct Message *)&res, sizeof res, 0);

				requester.clientID = clientID;
				addRequester(requester);
				addRequestToRequester(clientID, request);
				splitJob(request);
				break;
			case JOIN:
				if(req.clientID == 0) {
					clientID = getNewClientID();
					setConnection(&conn, clientID, clientSock);
					addConnection(conn);
					res = response(ACCEPT, clientID, 0, "");
					send(clientSock, (struct Message *)&res, sizeof res, 0);

					worker.clientID = clientID;
					addWorker(worker);
				}
				break;
			case DONE_NOT_FOUND:
				clientID = getRequesterFromRequest(req.requestID);
				sockfd = getSocketDesc(clientID);
				hash = getHash(req.other);
				package = getPackage(req.other);

				res = response(DONE_NOT_FOUND, clientID, req.requestID, hash);
				send(sockfd, (struct Message *)&res, sizeof res, 0);

				deleteJob(req.requestID, package);
				break;
			case DONE_FOUND:
				clientID = getRequesterFromRequest(req.requestID);
				sockfd = getSocketDesc(clientID);

				res = response(DONE_FOUND, clientID, req.requestID, req.other);
				send(sockfd, (struct Message *)&res, sizeof res, 0);

				removeJob(req.requestID);
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
			workerPos = getFirstEnableWorker();
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

				assignJob(&workerList[workerPos], &jobList[jobPos]);	
				printWorkerList();	// debug only
			}
		}
		sleep(5);	// after interval
	}
}