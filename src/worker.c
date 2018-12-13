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
#include "../lib/message.h"
#include "../lib/connection.h"
#include "../lib/error.h"
#include "../lib/helper.h"

#define MAX_LEN_BUF 30

int sockfd;
unsigned int clientID;
struct Message *jobQueue;
void *ThreadWork(void *threadArgs);


void initJobQueue() {
	jobQueue = (struct Message *) malloc(15 * sizeof(struct Message));
}

void addToQueue(struct Message job) {
    int i = 0;
	
	while(jobQueue[i++].requestID != 0);

	jobQueue[--i] = job;
}

struct Message getJobFromQueue() {
    int i = 0;
    struct Message job = jobQueue[0];
    //printf("%d\n",job.requestID);
    while (jobQueue[i+1].requestID != 0) {
        jobQueue[i] = jobQueue[i+1];
        i++;
    }
    jobQueue[i].requestID = 0;
    return job;
}

void signio_handler(int signo) {
	int n;
	struct Message res;
	struct Message req;
	char *hash;
	char *password;

	if((n = recv(sockfd, (struct Message *)&res, sizeof res, 0)) > 0) {
		switch(res.command) {
			case ACCEPT:
				clientID = res.clientID;
				printf("Now my ID = %u\n", clientID);
				break;
			case JOB: ;
                printf("Receive Job\n");
                addToQueue(res);
				break;
			default:
				break;
		}
	}
}

int main(int argc, char **argv)
{
    // TODO: advanced check for arguments list
    if (argc !=2) {
        exit(1);
    }

    pthread_t threadID;
    pthread_create(&threadID, NULL, ThreadWork, NULL);
    initJobQueue();

    sockfd = createTCPClientSocket(argv[1], SERV_PORT);

    if(fcntl(sockfd, F_SETFL, O_NONBLOCK | O_ASYNC) < 0)
        error(ERR_NON_BLK_ASYNC);

	signal(SIGIO, signio_handler);

	if(fcntl(sockfd, __F_SETOWN, getpid()) < 0)
        error(ERR_OWN_SOCKET);

    struct Message req;

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

void*ThreadWork(void* threadArgs) {
	pthread_detach(pthread_self());
    struct Message req;

    while (1) {
        struct Message job = getJobFromQueue();
        if (job.requestID != 0) {
            printf("Other: %s\n", job.other);
            char *password = solvePassword(job.other);
            printf("Password solve result: %s\n", password);

            if(password == NULL)
                req = response(DONE_NOT_FOUND, clientID, job.requestID, job.other);
            else {
				req = response(DONE_FOUND, clientID, job.requestID, password);
			}

            send(sockfd, (struct Message *)&req, sizeof req, 0);
        }
        sleep(5);
    }
}