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
#include <signal.h>
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
int debugFlag = 0;

void sigintHandler(int sig_num) 
{ 
    printf("\n Shutdown program \n");
	exit(0);
}

int main (int argc, char **argv) {
	pthread_t threadID;
	struct ThreadArgs *threadArgs;
	int listenfd, connfd;

	signal(SIGINT, sigintHandler);

	if(argc == 2) {
		if (strcmp(argv[1],"-d") == 0) {
			debugFlag = 1;
		}
	}

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

typedef enum TYPE_ {
	RECV,
	SEND
}TYPE;

void debugMsg(Message msg, TYPE type) {
	char s[15];
	strcpy(s, commandStr[msg.command]);
	int len = strlen(s)/sizeof(char);

	if(debugFlag) {
		if(type == RECV)
			printf("SERVER <----%*s%*s----- Client: %u\n",
				10+len/2,s,10-len/2,"",msg.clientID);
		else if(type == SEND)
			printf("SERVER -----%*s%*s----> Client: %u\n",
				10+len/2,s,10-len/2,"",msg.clientID);	
	}
}

void *ThreadRecv(void *threadArgs) {
	int clientSock = -1;

	pthread_detach(pthread_self());
	clientSock = ((struct ThreadArgs *) threadArgs) -> clntSock;

	// handle logic when receving message
	Message req, res;

	Connection conn = {0, 0};
	Requester requester = {0, {0, ""}};
	Worker worker = {0, 0};
	Request request = {0, ""};

	int clientID;
	char *other = malloc(MSG_OTHER_LENGTH);
	char *hash = malloc(HASH_LENGTH);
	char *password = malloc(PW_LENGTH);
	unsigned int package;
	int sockfd = 0;

	int n;
	while((n = recv(clientSock, (struct Message*)&req, sizeof req, 0)) > 0) {
		debugMsg(req, RECV);
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
				
				res = response(ACCEPT, clientID, request.requestID, request.hash);
				send(clientSock, (struct Message *)&res, sizeof res, 0);
				debugMsg(res, SEND);

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
					debugMsg(res, SEND);

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
				debugMsg(res, SEND);

				removeJob(req.requestID, package);
				break;
			case DONE_FOUND:
				clientID = getRequesterFromRequest(req.requestID);
				sockfd = getSocketDesc(clientID);

				res = response(DONE_FOUND, clientID, req.requestID, req.other);

				send(sockfd, (struct Message *)&res, sizeof res, 0);
				debugMsg(res, SEND);


				unsigned int *worker = getWorkerFromRequest(req.requestID);

				// not sending back to worker solved problem
				for (int i = 0; i < 26; i++)
					if(worker[i] == getClientIDFromSocket(clientSock)) {
						worker[i] = 0;
						break;
					}

				for (int i = 0; i < 26; i++) {
					if(worker[i] != 0 && worker[i] != clientID) {
						sockfd = getSocketDesc(worker[i]);
						res = response(DONE_FOUND, worker[i], req.requestID, "");

						send(sockfd, (struct Message *)&res, sizeof res, 0);
						debugMsg(res, SEND);
					}
				}
				removeAllJobs(req.requestID);
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

	Message res;
	char *other = malloc(MSG_OTHER_LENGTH);
	int sockfd;
	int jobPos;
	int workerPos;

	while(1) {
		jobPos = getFirstJob();
		if(jobPos != -1) {	// check if we have a job
			workerPos = getFirstEnableWorker();
			if(workerPos != -1) {	// check if we have a worker
				memset(&other, 0, sizeof other);
				other = getMsgFromJob(jobList[jobPos]);

				// printf("other = %s\n", other);

				sockfd = getSocketDesc(workerList[workerPos].clientID);

				if(sockfd != 0) {
					res = response(JOB, workerList[workerPos].clientID, jobList[jobPos].requestID, other);
				
					send(sockfd, (struct Message*)&res, sizeof res, 0);
					debugMsg(res, SEND);
				}

				assignJob(&workerList[workerPos], &jobList[jobPos]);	
				printWorkerList();	// debug only
			}
		}
		// sleep(1);	// after interval
	}
}