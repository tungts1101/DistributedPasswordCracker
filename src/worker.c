/* 2018/11/15
 * version 1.0
 * */

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <pthread.h>    // POSIX thread
#include "message.h"
#include "connection.h"
#include "error.h"
#include "helper.h"

#define MAX_LEN_BUF 30

int sockfd;
int clientID;
void *ThreadWork(void *threadArgs);
void *ThreadRecv(void *threadArgs);

struct Job {
    int requestID;
    char hash[MAX_LEN_BUF];
};

struct Job *jobQueue;

void initJobQueue() {
	jobQueue = (struct Job *) malloc(MAX_JOB_PER_WORKER * sizeof(struct Job));
}

void addToQueue(struct Job job) {
    int i = 0;
	
	while(jobQueue[i++].requestID != 0);

	jobQueue[--i] = job;
}

struct Job getJobFromQueue() {
    int i = 0;
    struct Job job = jobQueue[0];
    //printf("%d\n",job.requestID);
    while (jobQueue[i+1].requestID != 0) {
        jobQueue[i] = jobQueue[i+1];
        i++;
    }
    jobQueue[i].requestID = 0;
    return job;
}

void deleteSameJobFromQueue(int requestId) {
    int i = 0;
	
	while(jobQueue[i].requestID != 0) {
        if (jobQueue[i].requestID == requestId) {
            jobQueue[i].requestID = 0;
        }
        i++;
    }
}

void signio_handler(int signo) {
	int n;
	Message res;
	Message req;
	char *hash;
	char *password;
    struct Job job;

	if((n = recv(sockfd, (struct Message *)&res, sizeof res, 0)) > 0) {
		switch(res.command) {
			case ACCEPT:
				clientID = res.clientID;
				printf("Now my ID = %u\n", clientID);
				break;
			case JOB: ;
                printf("Receive Job\n");
                job.requestID = res.requestID;
                strcpy(job.hash,res.other);
                addToQueue(job);
                printf("===========\n");
				break;
            case DONE_FOUND: ;
                deleteSameJobFromQueue(res.requestID);
                break;
			default:
				break;
		}
	}
}

void sigintHandler(int sig_num) {
    Message msg = response(QUIT, clientID, 0, "");
    send(sockfd, (struct Message *)&msg, sizeof msg, 0);
    exit(0);
}

struct ThreadArgs {
    int clntSock;
};

int main(int argc, char **argv)
{
    // TODO: advanced check for arguments list
    if (argc !=2) {
        exit(1);
    }

    pthread_t threadID;
    // pthread_create(&threadID, NULL, ThreadWork, NULL);

    struct ThreadArgs *threadArgs;
    initJobQueue();

    sockfd = createTCPClientSocket(argv[1], SERV_PORT);

    signal(SIGINT, sigintHandler);

    threadArgs = (struct ThreadArgs *)malloc(sizeof(struct ThreadArgs));
    threadArgs -> clntSock = sockfd;
		
    pthread_create(&threadID, NULL, ThreadRecv, (void *) threadArgs);
    pthread_create(&threadID, NULL, ThreadWork, (void *) threadArgs);


    // if(fcntl(sockfd, F_SETFL, O_NONBLOCK | O_ASYNC))
    //     error(ERR_NON_BLK_ASYNC);

	// signal(SIGIO, signio_handler);

	// if(fcntl(sockfd, __F_SETOWN, getpid()) < 0)
    //     error(ERR_OWN_SOCKET);

    Message req;

    char* s = (char *)malloc(MAX_LEN_BUF * sizeof(char));
    char* temp_string = (char *)malloc(MAX_LEN_BUF * sizeof(char));
    char* splitString;
    int i = 0;
    char outputStringArray[MAX_LEN_BUF][MAX_LEN_BUF];


    while(fgets(s, MAX_LEN_BUF, stdin) != NULL) {
		i = 0;
        strcpy(temp_string,s);
        splitString = strtok(temp_string, "  \n");
        while (splitString != NULL) {
            strcpy(outputStringArray[i++], splitString);
            splitString = strtok(NULL, " ");
        }

		// Check loại connect
        if (strcmp(outputStringArray[0],"JOIN") == 0) {
            req = response(JOIN, clientID, 0, "");
            send(sockfd, (struct Message*)&req, sizeof req, 0);
        } else {
            printf("Wrong connection type\n");
        }
        memset(temp_string,0,strlen(temp_string));
    }

    exit(0);
}

void *ThreadRecv(void *threadArgs) {
    int clientSock = -1;

    pthread_detach(pthread_self());
    clientSock = ((struct ThreadArgs *) threadArgs) -> clntSock;

    int n;
	Message res;
	Message req;
	char *hash;
	char *password;
    struct Job job;

	while((n = recv(clientSock, (struct Message *)&res, sizeof res, 0)) > 0) {
		switch(res.command) {
			case ACCEPT:
				clientID = res.clientID;
				printf("Now my ID = %u\n", clientID);
				break;
			case JOB: ;
                printf("Receive Job\n");
                job.requestID = res.requestID;
                strcpy(job.hash,res.other);
                addToQueue(job);
                printf("===========\n");
				break;
            case DONE_FOUND: ;
                deleteSameJobFromQueue(res.requestID);
                break;
            case STOP:
                deleteSameJobFromQueue(res.requestID);
                break;
            case SHUTDOWN:
                exit(0);
                break;
			default:
				break;
		}
	}
}

void*ThreadWork(void* threadArgs) {
	int clientSock = -1;

    pthread_detach(pthread_self());
    clientSock = ((struct ThreadArgs *) threadArgs) -> clntSock;

    Message req;

    while (1) {
        // printf("...........................\n");
        struct Job job = getJobFromQueue();
        if (job.requestID != 0) {
            // printf("Other: %s\n", job.hash);
            char *password = solvePassword(job.hash);
            // printf("Password solve result: %s\n", password);

            if(password == NULL)
                req = response(DONE_NOT_FOUND, clientID, job.requestID, job.hash);
            else {
				req = response(DONE_FOUND, clientID, job.requestID, password);
                deleteSameJobFromQueue(job.requestID);
			}

            send(clientSock, (struct Message *)&req, sizeof req, 0);
        }
        sleep(1);
    }
}