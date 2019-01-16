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
#include <signal.h>	// signal()
#include <time.h>	// time_t, struct tm
#include <getopt.h>	// getopt()
#include "message.h"
#include "connection.h"
#include "config.h"
#include "error.h"
#include "structure.h"

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
int tries = 0;
int count = 0;
pthread_t threadID[MAX_PENDING+2];
FILE *logFp;

void handleCommandArg(int argc, char **argv);

int main (int argc, char **argv) {
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
	signal(SIGPIPE, SIG_IGN);

	handleCommandArg(argc, argv);	// handle command line argument

	init();	// init all data structure

	// for(int i = 0; i < MAX_PENDING - 1; i++)
	// 	for(int j = 0; j < MAX_REQUEST; j++)
	// 		printf("%d\n", requesterList[i].request[j].requestID);

	port = port ? port : SERV_PORT;
	listenfd = createTCPServerSocket(port);

	// spawn a thread for sending job purpose
	pthread_create(&threadID[count++], NULL, ThreadSend, NULL);

	while(1) {
		connfd = acceptTCPConnection(listenfd);

		threadArgs = (struct ThreadArgs *)malloc(sizeof(struct ThreadArgs));
		threadArgs -> clntSock = connfd;
		
		pthread_create(&threadID[count++], NULL, ThreadRecv, (void *) threadArgs);
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
	send(sockfd, (struct Message*)&msg, sizeof msg, 0);
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

	// wait for all child thread terminate
	for(int i = 0; i < MAX_PENDING + 2; i++)
		pthread_join(threadID[i], NULL);
	
	exit(0);
}

Connection conn = {0, 0};
Requester requester = {0, {0, ""}};
Worker worker = {0, 0};
Request request = {0, ""};

void handleHashRequest(int sockfd, Message msg) {
	request.requestID = ++requestNo;
	strcpy(request.hash, msg.other);

	int clientID = msg.clientID;
	if(clientID == 0) {
		clientID = getNewClientID();
		setConnection(&conn, clientID, sockfd);
		addConnection(conn);

		requester.clientID = clientID;
		addRequester(requester);
	}

	Message res = response(ACCEPT, clientID, request.requestID, request.hash);
	sendMsg(sockfd, res);

	addRequestToRequester(clientID, request);
	// printRequesterList();
	splitJob(request);
}

void handleJoinRequest(int sockfd, Message msg) {
	if(msg.clientID == 0) {
		int clientID = getNewClientID();
		setConnection(&conn, clientID, sockfd);
		addConnection(conn);

		Message res = response(ACCEPT, clientID, 0, "");
		sendMsg(sockfd, res);

		worker.clientID = clientID;
		addWorker(worker);
	}
}

void handleDNFRequest(int sockfd, Message msg) {
	int clientID = getRequesterFromRequest(msg.requestID);
	int clientSock = getSocketDesc(clientID);
	char *hash = getHash(msg.other);
	int package = getPackage(msg.other);

	Message res = response(DONE_NOT_FOUND, clientID, msg.requestID, hash);

	if(clientID != 0)
		sendMsg(clientSock, res);

	removeJob(msg.requestID, package);
}

void handleDFRequest(int sockfd, Message msg) {
	lock = 0;
	int clientID = getRequesterFromRequest(msg.requestID);
	int clientSock = getSocketDesc(clientID);

	Message res = response(DONE_FOUND, clientID, msg.requestID, msg.other);
	sendMsg(clientSock, res);

	// broadcast DONE_FOUND to all workers
	int *worker = getWorkerFromRequest(msg.requestID);

	for (int i = 0; i < 26; i++) {
		if(worker[i] != 0) {
			clientSock = getSocketDesc(worker[i]);
			res = response(DONE_FOUND, worker[i], msg.requestID, "");

			if(clientSock != sockfd)	// not sending back to worker solved problem
				sendMsg(clientSock, res);
		}
	}

	removeAllJobs(msg.requestID);
	lock = 1;
}

void handleQuitRequest(int sockfd, Message msg) {
	int clientID = getClientIDFromSocket(sockfd);
	recoverJob(clientID);
	removeWorker(clientID);
	removeConnection(clientID);
}

void handleStopRequest(int sockfd, Message msg) {
	if(msg.clientID != 0) {
		int clientID = getClientIDFromSocket(sockfd);
		int *request = getRequestFromRequester(clientID);


		int clientSock;
		Message res;

		for(int i = 0; i < MAX_REQUEST; i++)
			if(request[i] != 0) {
				lock = 0;
				int *worker = getWorkerFromRequest(request[i]);

				for (int j = 0; j < 26; j++) {
					if(worker[j] != 0) {
						clientSock = getSocketDesc(worker[i]);
						res = response(STOP, worker[j], request[i], "");
						sendMsg(clientSock, res);
					}
				}

				removeAllJobs(request[i]);
				lock = 1;
			}
		
		removeRequester(clientID);
		removeConnection(clientID);
	}
}

void *ThreadRecv(void *threadArgs) {
	int sockfd = -1;

	pthread_detach(pthread_self());
	sockfd = ((struct ThreadArgs *) threadArgs) -> clntSock;
	free(threadArgs);

	Message msg;

	while(recv(sockfd, (struct Message *)&msg, sizeof(msg), 0) > 0) {
		writeToLog(msg, RECV);

		if(debugFlag)
			debug(msg, RECV);

		switch(msg.command) {
			case HASH:
				handleHashRequest(sockfd, msg);
				break;
			case JOIN:
				handleJoinRequest(sockfd, msg);
				break;
			case DONE_NOT_FOUND:
				handleDNFRequest(sockfd, msg);
				break;
			case DONE_FOUND:
				handleDFRequest(sockfd, msg);
				break;
			case QUIT:
				handleQuitRequest(sockfd, msg);
				break;
			case STOP:
				handleStopRequest(sockfd, msg);
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