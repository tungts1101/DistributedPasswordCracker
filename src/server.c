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
#include <time.h>
#include <getopt.h>
#include "../lib/message.h"
#include "../lib/connection.h"
#include "../lib/config.h"
#include "../lib/error.h"
#include "../lib/structure.h"

void *ThreadRecv(void *threadArgs);
void *ThreadSend(void *threadArgs);
void sigintHandler(int sig_num);

struct ThreadArgs {
    int clntSock;
};

int requestNo = 0;	// keep track of number of requests
int debugFlag = 0;
int helpFlag = 0;
int lock = 1;	// handle data race between 2 threads
int port = 0;
FILE *logFp;

void handleCommandArg(int argc, char **argv);

int main (int argc, char **argv) {
	pthread_t threadID;
	struct ThreadArgs *threadArgs;
	int listenfd, connfd;
	int port = 0;

	// opening logs file for writing
	if((logFp = fopen("logs", "a+")) == NULL) {
		error(ERR_OPEN_LOGS);
	} else {
		time_t t = time(NULL);
		struct tm tm = *localtime(&t);
		fprintf(logFp, "%d-%d-%d %d:%d:%d\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
		fprintf(logFp, "------------------------------\n");
	}
	// ===============================

	signal(SIGINT, sigintHandler);	// handle Ctr+C

	handleCommandArg(argc, argv);	// handle command line argument

	init();	// init all data structure

	port = port ? port : SERV_PORT;
	listenfd = createTCPServerSocket(port);

	// spawn a thread for sending job purpose
	pthread_create(&threadID, NULL, ThreadSend, NULL);

	while(1) {
		connfd = acceptTCPConnection(listenfd);

		threadArgs = (struct ThreadArgs *)malloc(sizeof(struct ThreadArgs));
		threadArgs -> clntSock = connfd;
		
		pthread_create(&threadID, NULL, ThreadRecv, (void *) threadArgs);
	}

	// closing logs file
	if(fclose(logFp)) {
		error(ERR_CLOSE_LOGS);
	}
	// ===============================

	return 0;
}

void usage(FILE *fp, const char *path) {
	const char *basename = strrchr(path, '/');
	basename = basename ? basename + 1 : path;

	fprintf(fp, "usage: %s [OPTION]\n", basename);
	fprintf(fp, " -h\t"
				"Print this help and exit.\n");
	fprintf(fp, " -d\t"
				"Ouput debug information to stdout.\n");
	fprintf(fp, " -pPORT_NUMBER\t"
				"Assign a port number. Default is 3000.\n");
}

void handleCommandArg(int argc, char **argv) {
	int opt;
	while ((opt = getopt(argc, argv, "hdp:")) != -1) {
		switch (opt) {
			case 'h':
				helpFlag = 1;
				break;
			case 'd':
				debugFlag = 1;
				break;
			case 'p': ;
				char *ptr;
				port = (int) strtol(optarg, &ptr, 10);
				break;
			case '?':
				usage (stderr, argv[0]);
				exit(1);
			default:
				break;
		}
	}

	if(helpFlag) {
		usage(stdout, argv[0]);
		exit(0);
	}
}

typedef enum TYPE_ {
	RECV,
	SEND
}TYPE;	// differentiate send & recv message 

void debug(Message msg, TYPE type) {
	char s[15];
	strcpy(s, commandStr[msg.command]);
	int len = strlen(s)/sizeof(char);

	if(type == RECV)
		printf("SERVER <----%*s%*s----- Client: %u\n",
			10+len/2,s,10-len/2,"",msg.clientID);
	else if(type == SEND)
		printf("SERVER -----%*s%*s----> Client: %u\n",
			10+len/2,s,10-len/2,"",msg.clientID);	
}

void writeToLog(Message msg, TYPE type) {
	char s[15];
	strcpy(s, commandStr[msg.command]);
	int len = strlen(s)/sizeof(char);

	if(type == RECV)
		fprintf(logFp, "SERVER <----%*s%*s----- Client: %u\n",
			10+len/2,s,10-len/2,"",msg.clientID);
	else if(type == SEND)
		fprintf(logFp, "SERVER -----%*s%*s----> Client: %u\n",
			10+len/2,s,10-len/2,"",msg.clientID);
}

void sendMsg(int sockfd, Message msg) {
	if(send(sockfd, (struct Message *)&msg, sizeof(msg), 0) < 0)
		error(ERR_SEND_FAILED);

	writeToLog(msg, SEND);

	if(debugFlag)
		debug(msg, SEND);
}

void sigintHandler(int sig_num) { 
	Message res;
	int clientID;
	for(int i = 0; i < MAX_PENDING; i++) {
		if(connectionList[i].clientID != 0) {
			clientID = connectionList[i].clientID;
			res = response(SHUTDOWN, clientID, 0, "");
			send(connectionList[i].sockfd, (struct Message *)&res, sizeof(res), 0);
			debug(res, SEND);
			removeConnection(clientID);
		}
	}

	exit(0);
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
	while((n = recv(clientSock, (struct Message *)&req, sizeof(req), 0)) > 0) {
		writeToLog(req, RECV);

		if(debugFlag)
			debug(req, RECV);

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
				sendMsg(clientSock, res);

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
					sendMsg(clientSock, res);

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
				sendMsg(sockfd, res);

				removeJob(req.requestID, package);
				break;
			case DONE_FOUND:
				lock = 0;
				clientID = getRequesterFromRequest(req.requestID);
				sockfd = getSocketDesc(clientID);

				res = response(DONE_FOUND, clientID, req.requestID, req.other);
				sendMsg(sockfd, res);

				// broadcast DONE_FOUND to all workers
				unsigned int *worker = getWorkerFromRequest(req.requestID);

				for (int i = 0; i < 26; i++) {
					if(worker[i] != 0) {
						sockfd = getSocketDesc(worker[i]);
						res = response(DONE_FOUND, worker[i], req.requestID, "");

						if(sockfd != clientSock)	// not sending back to worker solved problem
							sendMsg(sockfd, res);
					}
				}

				removeAllJobs(req.requestID);
				lock = 1;
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
				sockfd = getSocketDesc(workerList[workerPos].clientID);
				res = response(JOB, workerList[workerPos].clientID, jobList[jobPos].requestID, other);
			
				if(lock == 1) {
					sendMsg(sockfd, res);
					assignJob(&workerList[workerPos], &jobList[jobPos]);
				}
			}
		}
	}
}